#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#ifdef WITH_MPI
#include <mpi.h>
#endif

#ifdef WITH_SIGNAL
#include <signal.h>
#endif
#ifdef WITH_SYS_RESOURCE
#include <sys/resource.h>
#endif

#include "llvm/IR/LLVMContext.h"
#include <llvm/IR/Value.h>
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/SourceMgr.h"

#include <phasar/DB/ProjectIRDB.h>
#include <phasar/PhasarLLVM/IfdsIde/Solver/LLVMIFDSSolver.h>
#include <phasar/PhasarLLVM/Plugins/Interfaces/IfdsIde/IFDSTabulationProblemPlugin.h>
#include <phasar/PhasarLLVM/Pointer/LLVMTypeHierarchy.h>

#include "AnalysisController.h"
#include "ApplicationRunContext.h"
#include "GraphSlicer.h"
#include "LazyMPIRMAUseProblem.h"
#include "LazyWindowSheet.h"
#include "LLVMBasedMICFG.h"
#include "MPIRMAUseProblem.h"
#include "Settings.h"
#include "SheetController.h"
#include "StatisticsProblem.h"
#include "StatisticsSheet.h"
#include "SwitchConverter.h"
#include "UnlockMapGenerator.h"
#include "WindowSheet.h"
#include "WormholeMapGenerator.h"

bool parseArguments (int argc, char **argv, Settings &settings) {
  if (argc < 2) {
    return false;
  }

  for (int i = 1; i < argc; ++i) {
    std::string arg{argv[i]};

    if (0 == arg.compare("-B1")) {
      settings.benchmarkMode = 1;
    } else if (0 == arg.compare("-B2")) {
      settings.benchmarkMode = 2;
    } else if (0 == arg.compare("-d")) {
      settings.showPhasarDetails = true;
    } else if (0 == arg.compare("-e")) {
      if (i + 1 < argc) {
        settings.entryPoint.assign(argv[i+1]);
      } else {
        return false;
      }
    } else if (0 == arg.compare("-L")) {
      settings.ifdsMode = Settings::IFDSMode::Lazy;
    } else if (0 == arg.compare("-o")) {
      if (i + 1 < argc) {
        settings.pathPrefix.assign(argv[i+1]);
      } else {
        return false;
      }
    } else if (0 == arg.compare("-OI")) {
      settings.integerMode = Settings::IntegerMode::Static;
    } else if (0 == arg.compare("-OR")) {
      settings.integerMode = Settings::IntegerMode::None;
    } else if (0 == arg.compare("-OW")) {
      settings.findWormholes = true;
    } else if (0 == arg.compare("-s")) {
      settings.statisticsMode = Settings::Statistics::Only;
    } else if (0 == arg.compare("-S")) {
      settings.statisticsMode = Settings::Statistics::Skip;
    } else if (0 == arg.compare("-v")) {
      settings.friendlyExit = Settings::FriendlyExit::Version;

      return true;
    } else if (0 == arg.compare("-V")) {
      settings.tickWindowProcess = true;
    } else if (0 == arg.compare("-w")) {
      if (i + 1 < argc) {
        if (sscanf(argv[i+1], "%d", &(settings.ithWindow)) != 1) {
          return false;
        }
      } else {
        return false;
      }
    } else if (0 == arg.compare("-h") || 0 == arg.compare("--help")) {
      settings.friendlyExit = Settings::FriendlyExit::Help;

      return true;
    } else if (i + 1 == argc && '-' != arg.front()) {
      settings.moduleFile.assign(argv[i]);
    }
  }

  if ( settings.tickWindowProcess
    && Settings::Statistics::Skip == settings.statisticsMode
  ) {
    return false;
  }

  return !settings.moduleFile.empty();
}

void showHelp () {
  std::cout << std::endl
    << "mpi_analysis [options] module_file" << std::endl
    << std::endl
    << "Available options:" << std::endl
    << "\t-B1|2: benchmark mode (1: no CSV files, 2: no CSV files, no post-analysis)" << std::endl
    << "\t-d: show Phasar per-instruction fact analysis before post-analysis" << std::endl
    << "\t-e ENTRY_POINT: the entry point function (default \"main\")" << std::endl
    << "\t-h|--help: what you read right now" << std::endl
    << "\t-L: lazy IFDS path sensitivity (reduced features)" << std::endl
    << "\t-o PREFIX: path prefix for CSV output files" << std::endl
    << "\t-OI: assume integers used for rank comparisions not to ever change" << std::endl
    << "\t-OR: skip inter-rank analyses (includes -OI)" << std::endl
    << "\t-OW: look for wormholes to skip presumable irrelevant code sections" << std::endl
    << "\t-s: exit after preflight, just issuing scan statistics" << std::endl
    << "\t-S: skip preflight" << std::endl
    << "\t-v: show version information" << std::endl
    << "\t-V: attempt to show window process, can't be used with -S" << std::endl
    << "\t-w i: restrict analysis to the i-th window, starting at 1" << std::endl
    << std::endl;
}

void showVersion () {
  std::cout << "1.0.0";

#ifdef WITH_MPI
  char mpiVersionString[MPI_MAX_LIBRARY_VERSION_STRING];
  int mpiVersionStringLength = 0;

  MPI_Get_library_version(mpiVersionString, &mpiVersionStringLength);
  if (mpiVersionStringLength > 0) {
    std::string mpiVersion{mpiVersionString, static_cast<size_t>(mpiVersionStringLength)};

    std::cout << std::endl << "built with MPI " << mpiVersion << std::endl;
  }
#endif
}

ApplicationRunContext *runContext = nullptr;

void windowProcessTicker (int signal) {
  if ( runContext->isTickingEnabled
    && runContext->isProcessingWindows
  ) {
    if (SIGUSR1 == signal) {
      runContext->genProcessCounter += 1;
    } else if (SIGUSR2 == signal) {
      runContext->killProcessCounter += 1;
    } else {
      return;
    }

    /**
     * Adjust the counter by discarded paths that will never reach the end,
     * assuming their forks will.
     */
    uint64_t genCounter = (runContext->genProcessCounter + 1) / 2;

    if (runContext->isFeasibleToCount) {
      std::cout << "\rWindow: "
        << runContext->killProcessCounter
        << " / "
        << genCounter
        << " (2^" << runContext->upperBoundExponent << " * " << runContext->factFactor << ")";
    } else {
      std::cout << "\rWindow: "
        << genCounter;

      if (static_cast<uint64_t>(-1) == runContext->genProcessCounter) {
        std::cout << "+";
      }
    }

    std::cout << std::flush;
  }
}

int main (int argc, char **argv) {
  std::vector<std::string> entryPoints{"main"};
  Settings settings;

  if (parseArguments(argc, argv, settings)) {
    if (settings.wantsHelpfulExit()) {
      switch (settings.friendlyExit) {
        case Settings::FriendlyExit::Help:
          showHelp();
          break;
        case Settings::FriendlyExit::Version:
          showVersion();
          break;
        default:
          break;
      }

      return 0;
    }
  } else {
    showHelp();

    return 1;
  }

  ApplicationRunContext applicationContext;

#ifdef WITH_SIGNAL
  /**
   * Poor man's approach to allow counting terminated windows from
   * whereever this happens down the code.
   */
  runContext = &applicationContext;
  applicationContext.isTickingEnabled = settings.tickWindowProcess;

  signal(SIGUSR1, windowProcessTicker);
  signal(SIGUSR2, windowProcessTicker);
#endif

#ifdef WITH_SYS_RESOURCE
  rlimit limit;
  if (0 == getrlimit(RLIMIT_STACK, &limit)) {
    if (static_cast<size_t>(-1) != limit.rlim_cur) {
      std::cout << std::endl
        << "Warning: A stack size limit may easily abort the program. Consider an adjustment via `ulimit -s`."
        << std::endl
        << std::endl;
    }
  }
#endif

  if (!settings.entryPoint.empty()) {
    entryPoints = {settings.entryPoint};
  }

  // Phasar cleans that up!
  auto llvmContext = new llvm::LLVMContext;
  llvm::SMDiagnostic diagnostic;

  auto module = llvm::parseIRFile(llvm::StringRef{settings.moduleFile}, diagnostic, *llvmContext);

  if (nullptr == module) {
    std::cerr << "Error: Could not lot the given file." << std::endl;
    return 1;
  }

  SwitchConverter switchConverter{*module.get(), *llvmContext};
  switchConverter.rewrite();

  if (settings.findWormholes) {
    WormholeMapGenerator wormholeMapGenerator{*module.get(), settings};
    auto wormholeMap = wormholeMapGenerator.createWormholeMap();
    WormholeMapGenerator::OperationWormholeMap operationWormholeMap;

    for (auto &pair : wormholeMap) {
      operationWormholeMap.emplace(
          std::piecewise_construct
        , std::forward_as_tuple(const_cast<llvm::BasicBlock*>(pair.first))
        , std::forward_as_tuple(pair.second)
      );
    }

    GraphSlicer graphSlicer{operationWormholeMap};
    graphSlicer.slice();
  }

  psr::initializeLogger(false);
  psr::ProjectIRDB projectIRDB(psr::IRDBOptions::WPA);
  projectIRDB.insertModule(std::move(module));
  projectIRDB.preprocessIR();

  if (projectIRDB.getFunction(entryPoints.front())) {
    psr::LLVMTypeHierarchy hierarchy{projectIRDB};
    LLVMBasedMICFG icfg{hierarchy, projectIRDB, entryPoints};

    UnlockMapGenerator unlockMapGen{projectIRDB};
    auto mapPair = unlockMapGen.createMap();
    bool warnOnErrors = false;

    if (
         Settings::Statistics::Skip != settings.statisticsMode
      && settings.benchmarkMode < 2
    ) {
      StatisticsProblem statisticsProblem{
          icfg
        , mapPair.first
        , entryPoints
        , settings.integerMode
      };
      psr::LLVMIFDSSolver<const llvm::Value*, psr::LLVMBasedICFG&> presolver{
          *reinterpret_cast<psr::IFDSTabulationProblemPlugin*>(&statisticsProblem)
        , false
      };
      presolver.solve();
      statisticsProblem.finalize();

      auto data = statisticsProblem.getRecord();

      SheetController<StatisticsSheet> statSheetCtrl{"statistics", settings};
      statSheetCtrl.getSheet()->writeStatistitcs(data);

      std::cout << std::endl
        << "Graph analysis:" << std::endl
        << "  number of instructions: " << data.numInstructions << " (" << data.numInstructionsTotal << " in total)" << std::endl
        << "  number of fork-causing branchings: " << data.numBranches << std::endl
        << "  number of created windows: " << data.numWindows << std::endl
        << "  number of `MPI_Win_` calls: " << data.numWinCalls << std::endl
        << "  number of const. assigned ints: " << data.numIntegers << std::endl
        << std::endl;

      if (data.numBranches > 12 && Settings::IFDSMode::Lazy != settings.ifdsMode) {
        std::cout << "Note: You may want to consider using the lazy path sensitivity (-L). " << std::endl;
      }

      bool feasibleCounting = false;

      /**
       * Assuming a 64bit counter:
       *  * up to 2^64-1 terminating paths
       *  * half the number - 1 may have been discarded before - goes on top.
       */
      uint64_t numWindowsExponent = ceil(log(data.numWindows) / log(2)), result = 0;

      if ( !__builtin_add_overflow(numWindowsExponent, data.numBranches, &result)
        && result <= 62
      ) {
        feasibleCounting = true;
      }

      if (feasibleCounting) {
        applicationContext.upperBoundExponent = data.numBranches;
        applicationContext.factFactor = data.numWindows;
      } else {
        applicationContext.isFeasibleToCount = false;
      }
    }

    if (Settings::Statistics::Only == settings.statisticsMode) {
      return 0;
    }

    if (Settings::IFDSMode::Lazy == settings.ifdsMode) {
      LazyMPIRMAUseProblem problem{
          icfg
        , mapPair.second
        , entryPoints
        , settings
      };

      applicationContext.isProcessingWindows = true;
      psr::LLVMIFDSSolver<const llvm::Value*, psr::LLVMBasedICFG&> solver{
          *reinterpret_cast<psr::IFDSTabulationProblemPlugin*>(&problem)
        , settings.showPhasarDetails
      };
      solver.solve();
      applicationContext.isProcessingWindows = false;

      std::cout << std::endl;

      SheetController<LazyWindowSheet> windowSheetCtrl{"windows", settings};
      auto &facts = problem.getFacts();

      for (auto &fact : facts) {
        if (fact->isType(LazyFact::Type::Win) && !fact->isDroppedForForking()) {
          if (fact->isLookingErrorneous()) {
            warnOnErrors = true;
          }

          windowSheetCtrl.getSheet()->writeWindowFact(fact.get());
        }
      }

      windowSheetCtrl.close();
    } else {
      MPIRMAUseProblem problem{
          icfg
        , mapPair.first
        , entryPoints
        , settings
      };

      applicationContext.isProcessingWindows = true;
      psr::LLVMIFDSSolver<const llvm::Value*, psr::LLVMBasedICFG&> solver{
          *reinterpret_cast<psr::IFDSTabulationProblemPlugin*>(&problem)
        , settings.showPhasarDetails
      };
      solver.solve();
      applicationContext.isProcessingWindows = false;

      SheetController<WindowSheet> windowSheetCtrl{"windows", settings};

      uint64_t totalFactsAlive = 0;
      uint64_t finishedPathFacts = 0;

      auto &facts = problem.getFacts();
      for (auto &fact : facts) {
        switch (fact->getType()) {
          case Fact::Type::IntConst:
            totalFactsAlive += 1;
            break;
          case Fact::Type::Win:
            if (!fact->isDroppedForForking()) {
              totalFactsAlive += 1;
              finishedPathFacts += 1;
            }
            break;
          case Fact::Type::WinGhost:
            totalFactsAlive += 1;
            break;
          default:
            // Ignore Comm for just looking for exploding facts.
            break;
        }
      }

      if (Settings::Statistics::Skip != settings.statisticsMode) {
        std::cout << std::endl
          << "Post IFDS-analysis" << std::endl
          << "  # facts (win, win-ghosts, ints): " << totalFactsAlive << std::endl
          << "  # paths: " << finishedPathFacts << std::endl
          << std::endl;
      }

      if (settings.benchmarkMode < 2) {
        AnalysisController analysisController{
            problem.getFacts()
          , settings
        };
        analysisController.run();

        for (auto &fact : facts) {
          if (fact->isType(Fact::Type::Win) && !fact->isDroppedForForking()) {
            if (fact->isLookingErrorneous()) {
              warnOnErrors = true;
            }

            windowSheetCtrl.getSheet()->writeWindowFact(fact.get());
          }
        }
      }

      windowSheetCtrl.close();
    }

    if (warnOnErrors) {
      std::cout << "=> Attention: a violation has been detected, please check `windows.csv`." << std::endl;
    } else {
      std::cout << "=> No violations detected." << std::endl;
    }

  } else {
    std::cerr << "In file "
      << argv[argc-1]
      << ", there is no such entry point: "
      << entryPoints.front()
      << std::endl;

    return 1;
  }

  return 0;
}
