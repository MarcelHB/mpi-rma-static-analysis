#ifdef WITH_SIGNAL
#include <signal.h>
#endif

#ifdef WITH_MPI
#include <mpi.h>
#endif

#include "llvm/IR/Constants.h"
#include "llvm/IR/GlobalVariable.h"

#include "WindowCreator.h"

WindowCreator::WindowCreator (
    llvm::ImmutableCallSite CallSite
  , MPIRMAUseProblem &Problem
  , Fact *ZV
  , MPISymbol Symbol
) : zeroValue(ZV)
  , callSite(CallSite)
  , problem(Problem)
  , symbol(Symbol)
{}

std::set<Fact*> WindowCreator::computeTargets (Fact* value) {
  if (value->isType(Fact::Type::Comm)) {
    if (value->knowsValue(callSite.getArgument(getCommArgPosition()))) {
      if (zeroValue->canCreateWindowFact()) {
        Fact *generatedFact =
          problem.makeInitialFact(
              Fact::Type::Win
            , zeroValue
          );

        generatedFact->setGenerationData(
            callSite.getInstruction()
          , callSite.getArgument(getCommArgPosition())
          , value
        );

#ifdef WITH_SIGNAL
        raise(SIGUSR1);
#endif

        generatedFact->addKnownValue(callSite.getArgument(getWindowArgPosition()), true);
        setWindowData(generatedFact);

        zeroValue->registerWindowPassage(callSite.getInstruction(), generatedFact);
        moveGhostsToFact(generatedFact);
        assignOrderToFact(generatedFact);

        zeroValue->increaseWindowCounter();

        return {value, generatedFact};
      } else {
        zeroValue->increaseWindowCounter();
      }
    }
  } else if (value->isType(Fact::Type::Null)) {
    if (zeroValue->canCreateWindowFact()) {
      auto constant = llvm::dyn_cast<llvm::ConstantInt>(callSite.getArgument(getCommArgPosition()));

      // Fortran module variables/constants.
      if (!constant) {
        if (auto bitcast = llvm::dyn_cast<llvm::BitCastInst>(callSite.getArgument(getCommArgPosition()))) {
          auto target = bitcast->getOperand(0);
          auto global = llvm::dyn_cast<llvm::GlobalVariable>(target);

          if (global && global->isConstant() && !global->getInitializer()->isNullValue()) {
            constant = llvm::dyn_cast<llvm::ConstantInt>(global->getInitializer());
          }
        }
      }

      if (constant) {
        Fact *generatedFact =
          problem.makeInitialFact(
              Fact::Type::Win
            , zeroValue
          );

        generatedFact->setGenerationData(
            callSite.getInstruction()
          , callSite.getArgument(getCommArgPosition())
          , nullptr
        );

#ifdef WITH_SIGNAL
        raise(SIGUSR1);
#endif

        generatedFact->addKnownValue(callSite.getArgument(getWindowArgPosition()), true);
        setWindowData(generatedFact);

        zeroValue->registerWindowPassage(callSite.getInstruction(), generatedFact);
        moveGhostsToFact(generatedFact);
        assignOrderToFact(generatedFact);

        zeroValue->increaseWindowCounter();

        return {value, generatedFact};
      }
    } else {
      zeroValue->increaseWindowCounter();
    }
  } else if (value->isType(Fact::Type::WinGhost)) {
    value->markForPurging();

    zeroValue->registerGhostPassage(callSite.getInstruction(), value);
    auto windowFacts = zeroValue->getPassedWindows(callSite.getInstruction());

    if (nullptr != windowFacts) {
      for (auto fact : *windowFacts) {
        fact->addAncestry(value->getGeneratorUnit());
      }
    }

    return {};
  } else if (value->isType(Fact::Type::Win)) {
    auto windowFacts = zeroValue->getPassedWindows(callSite.getInstruction());
    zeroValue->registerParallelWindowPassage(callSite.getInstruction(), value);

    if (nullptr != windowFacts) {
      for (auto fact : *windowFacts) {
        fact->increaseUnitCount(value->getUnitCount());
      }
    }
  }

  return {value};
}

void WindowCreator::assignOrderToFact (Fact *newFact) const {
  auto otherWindowFacts = zeroValue->getPassedGhosts(callSite.getInstruction());

  if (nullptr != otherWindowFacts) {
    for (auto otherWindow : *otherWindowFacts) {
      newFact->increaseUnitCount(otherWindow->getUnitCount());
    }
  }
}

unsigned WindowCreator::getCommArgPosition () const {
  switch (symbol) {
    case MPISymbol::MPIWinAllocate:
    case MPISymbol::MPIWinAllocateShared: return 3;
    case MPISymbol::MPIWinCreate: return 4;
    case MPISymbol::MPIWinCreateDynamic: return 1;
    default: return -1;
  }
}

unsigned WindowCreator::getInfoArgPosition () const {
  switch (symbol) {
    case MPISymbol::MPIWinAllocate:
    case MPISymbol::MPIWinAllocateShared: return 2;
    case MPISymbol::MPIWinCreate: return 3;
    case MPISymbol::MPIWinCreateDynamic: return 0;
    default: return -1;
  }
}

unsigned WindowCreator::getMemArgPosition () const {
  switch (symbol) {
    case MPISymbol::MPIWinAllocate:
    case MPISymbol::MPIWinAllocateShared: return 4;
    case MPISymbol::MPIWinCreate: return 0;
    default: return -1;
  }
}

unsigned WindowCreator::getWindowArgPosition () const {
  switch (symbol) {
    case MPISymbol::MPIWinAllocate:
    case MPISymbol::MPIWinAllocateShared:
    case MPISymbol::MPIWinCreate: return 5;
    case MPISymbol::MPIWinCreateDynamic: return 2;
    default: return -1;
  }
}

void WindowCreator::moveGhostsToFact (Fact *newFact) const {
  auto ghostFacts = zeroValue->getPassedGhosts(callSite.getInstruction());

  if (nullptr != ghostFacts) {
    for (auto ghost : *ghostFacts) {
      newFact->addAncestry(ghost->getGeneratorUnit());
    }
  }
}

void WindowCreator::setWindowData (Fact *newFact) const {
  const llvm::Value *infoHandle = callSite.getArgument(getInfoArgPosition());

#ifdef WITH_MPI
  if (auto constant = llvm::dyn_cast<llvm::ConstantInt>(infoHandle)) {
    int64_t constantValue = constant->getSExtValue();

    if (static_cast<int64_t>(MPI_INFO_NULL) == constantValue) {
      infoHandle = nullptr;
    }
  }
#endif

  newFact->setWindowInformation(symbol, infoHandle);

  if (MPISymbol::MPIWinCreateDynamic != symbol) {
    newFact->addKnownAddress(callSite.getArgument(getMemArgPosition()), true);
  }
}
