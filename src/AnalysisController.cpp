#include <iostream>

#include "AnalysisController.h"
#include "CommSynchronizationCheck.h"
#include "ConcurrentCommCheck.h"
#include "DynamicMemoryRequestCheck.h"
#include "EpochMatcher.h"

#ifdef WITH_MPI
#include "FenceFlagCheck.h"
#endif

#include "PathGrouper.h"
#include "RankAttributionAndContradictionCheck.h"
#include "SheetController.h"
#include "UnsafeAccessCheck.h"
#include "WindowModeCheck.h"
#include "ViolationsSheet.h"

AnalysisController::AnalysisController (
    const MPIRMAUseProblem::FactsCollection &Facts
  , const Settings &S
)
  : facts(Facts)
  , settings(S)
{}

void AnalysisController::run () const {
  auto &out = std::cout;

  SheetController<ViolationsSheet> violationsSheetCtrl{"violations", settings};

  /**
   * Order checks by what fits best w.r.t. calc. complexity:
   *
   * 1. Purely intra-path tests that need no grouping at all.
   * 2. Any inter-path tests that require prior grouping of valid leftovers.
   * 3. Skippable inter-rank analyses.
   */

  out << "> Dynamic memory request check ..." << std::endl;

  for (auto &fact : facts) {
    if (fact->isType(Fact::Type::Win)) {
      auto window = fact.get();
      auto SM = window->getPath();

      if ( nullptr == SM->getViolatingInstruction()
        && SM->hasFinalState()
      ) {
        DynamicMemoryRequestCheck check{window};
        auto violatingInstruction = check.validate();

        if (nullptr != violatingInstruction) {
          auto path = window->getPath();
          path->setViolatingInstruction(violatingInstruction);

          violationsSheetCtrl.getSheet()->writeViolation(
              window
            , violatingInstruction
            , "illegal_dynamic_memory_use"
          );
        }
      }
    }
  }

  out << "> Unsafe local memory access check ..." << std::endl;

  for (auto &fact : facts) {
    if ( fact->isType(Fact::Type::Win)) {
      auto window = fact.get();
      auto SM = window->getPath();

      if ( nullptr == SM->getViolatingInstruction()
        && SM->hasFinalState()
      ) {
        UnsafeAccessCheck check{window};
        auto violatingInstruction = check.validate();

        if (nullptr != violatingInstruction) {
          auto path = window->getPath();
          path->setViolatingInstruction(violatingInstruction);

          violationsSheetCtrl.getSheet()->writeViolation(
              window
            , violatingInstruction
            , "unsafe_local_memory_access"
          );
        }
      }
    }
  }

  PathGrouper grouper(facts);
  auto result = grouper.getOrderedWindows();
  bool doNewGroupRequest = false;

  out << "> Window mode check ..." << std::endl;

  for (auto &epoch : result) {
    for (auto &commSet : epoch) {
      for (auto &rankSet : commSet) {
        WindowModeCheck check{rankSet.second};

        if (!check.validate()) {
          doNewGroupRequest = true;

          for (auto window : rankSet.second) {
            auto path = window->getPath();
            auto &transitions = path->getTransitions();
            auto firstInstruction = transitions.front().instruction;

            path->setViolatingInstruction(firstInstruction);

            violationsSheetCtrl.getSheet()->writeViolation(
                window
              , firstInstruction
              , "window_flavors_incompatible"
            );
          }
        }
      }
    }
  }

  if (doNewGroupRequest) {
    result = grouper.getOrderedWindows();
    doNewGroupRequest = false;
  }

  out << "> Stuck analysis ..." << std::endl;

  for (auto &epoch : result) {
    for (auto &commSet : epoch) {
      for (auto &rankSet : commSet) {
        CommSynchronizationCheck check{rankSet.second};
        auto stuckFactResult = check.validate();

        for (auto &result : stuckFactResult) {
          auto path = result.fact->getPath();
          path->setViolatingInstruction(result.instruction);

          doNewGroupRequest = true;

          violationsSheetCtrl.getSheet()->writeViolation(
              result.fact
            , result.instruction
            , "waits_forever"
          );
        }
      }
    }
  }


#ifdef WITH_MPI
  if (doNewGroupRequest) {
    result = grouper.getOrderedWindows();
    doNewGroupRequest = false;
  }

  out << "> Fence flag check ..." << std::endl;

  for (auto &epoch : result) {
    for (auto &commSet : epoch) {
      for (auto &rankSet : commSet) {
        FenceFlagCheck check{rankSet.second};
        auto violations = check.validate();

        for (auto &validation : violations) {
          auto path = validation.window->getPath();
          path->setViolatingInstruction(validation.instruction);

          doNewGroupRequest = true;

          violationsSheetCtrl.getSheet()->writeViolation(
              validation.window
            , validation.instruction
            , "fence_flags_mismatch"
          );
        }
      }
    }
  }
#endif

  if (Settings::IntegerMode::None != settings.integerMode) {
    if (doNewGroupRequest) {
      result = grouper.getOrderedWindows();
      doNewGroupRequest = false;
    }
    out << "> Rank-branch contradiction filter ..." << std::endl;

    for (auto &epoch : result) {
      for (auto &commSet : epoch) {
        for (auto &rankSet : commSet) {
          for (auto window : rankSet.second) {
            RankAttributionAndContradictionCheck check{window};
            auto violatingInstruction = check.attributeAndValidate();

            if (nullptr != violatingInstruction) {
              auto path = window->getPath();
              path->setViolatingInstruction(violatingInstruction);

              doNewGroupRequest = true;
              violationsSheetCtrl.getSheet()->writeViolation(
                  window
                , violatingInstruction
                , "elimination_by_rank"
              );
            }
          }
        }
      }
    }
  }



  if (Settings::IntegerMode::None == settings.integerMode) {
    return;
  }

  if (doNewGroupRequest) {
    result = grouper.getOrderedWindows();
    doNewGroupRequest = false;
  }

  out << "> Matching plausible epochs ..." << std::endl;

  EpochMatcher::WindowGuides factGuides{};

  for (auto &epoch : result) {
    for (auto &commSet : epoch) {
      for (auto &rankSet : commSet) {
        EpochMatcher epochMatcher{rankSet.second};
        EpochMatcher::WindowGuides guides = epochMatcher.match();

        for (auto &guidePair : guides) {
          auto &guide = guidePair.second;
          auto window = guide.window;

          if (nullptr == guide.violatingInstruction) {
            factGuides[window] = std::move(guidePair.second);
          } else {
            auto path = window->getPath();
            path->setViolatingInstruction(guide.violatingInstruction);

            doNewGroupRequest = true;

            if (guide.violation == EpochMatcher::Violation::NotAllAvailable) {
              violationsSheetCtrl.getSheet()->writeViolation(
                  window
                , guide.violatingInstruction
                , "illegal_by_other_rank"
              );
            } else {
              violationsSheetCtrl.getSheet()->writeViolation(
                  window
                , guide.violatingInstruction
                , "no_corresponding_rank"
              );
            }
          }
        }
      }
    }
  }

#if 0
  for (auto &pair : factGuides) {
    auto &guide = pair.second;

    for (auto &matchedInstruction : guide.instructions) {
      for (auto &otherPair : matchedInstruction.matchingInstructions) {
        for (auto &otherInstruction : otherPair.second) {
          out << "Win "
            << pair.first->getId()
            << ": "
            << psr::llvmIRToString(matchedInstruction.instruction.instruction)
            << " -> ("
            << otherPair.first->getId()
            << ") "
            << psr::llvmIRToString(otherInstruction.instruction)
            << std::endl;
        }
      }
    }
  }
#endif

  if (doNewGroupRequest) {
    result = grouper.getOrderedWindows();
    doNewGroupRequest = false;

    for (auto &epoch : result) {
      for (auto &commSet : epoch) {
        for (auto &rankSet : commSet) {
          EpochMatcher epochMatcher{rankSet.second};
          EpochMatcher::WindowGuides guides = epochMatcher.match();

          for (auto &guidePair : guides) {
            auto &guide = guidePair.second;
            auto window = guide.window;

            if (nullptr == guide.violatingInstruction) {
              factGuides[window] = std::move(guidePair.second);
            }
          }
        }
      }
    }
  }

  out << "> Concurrent local write analysis ..." << std::endl;

  ConcurrentCommCheck check{factGuides};
  auto concurrentCommResult = check.validate();

  for (auto &pair : concurrentCommResult) {
    auto window = pair.first;
    auto violatingInstruction = pair.second;
    auto path = window->getPath();

    path->setViolatingInstruction(violatingInstruction);
    doNewGroupRequest = true;

    violationsSheetCtrl.getSheet()->writeViolation(
        window
      , violatingInstruction
      , "concurrent_local_access"
    );
  }
}
