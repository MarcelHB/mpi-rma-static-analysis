#include "llvm/IR/CallSite.h"
#include "llvm/IR/Constants.h"

#include "EpochMatcher.h"

EpochMatcher::EpochMatcher (std::set<Fact*> &Facts)
  : facts(Facts)
{}

using Epoch = EpochMatcher::Epoch;

void EpochMatcher::coExecute (Context &context) const {
  for (auto &pair : context.guides) {
    auto &guide = pair.second;

    for (auto &otherPair : context.guides) {
      auto &otherGuide = otherPair.second;

      if (otherGuide.window == guide.window) {
        continue;
      }

      uint64_t instructionIdx = 0, otherInstructionIdx = 0;
      uint64_t lastStartIdx = 0, otherLastStartIdx = 0;
      MPISymbol waitSymbol = MPISymbol::Null, otherWaitSymbol = MPISymbol::Null;

      while (true) {
        lastStartIdx = instructionIdx;

        for (; instructionIdx < guide.instructions.size(); ++instructionIdx) {
          auto &instruction = guide.instructions[instructionIdx];

          if (MPISymbol::Null != getBlockingInstruction(instruction.instruction)) {
            waitSymbol = instruction.instruction.symbol;
            instructionIdx += 1;
            break;
          }
        }

        otherLastStartIdx = otherInstructionIdx;

        for (; otherInstructionIdx < otherGuide.instructions.size(); ++otherInstructionIdx) {
          auto &instruction = otherGuide.instructions[otherInstructionIdx];

          if (MPISymbol::Null != getBlockingInstruction(instruction.instruction)) {
            otherWaitSymbol = instruction.instruction.symbol;

            if ( (MPISymbol::MPIBarrier == waitSymbol && otherWaitSymbol == waitSymbol)
              || (MPISymbol::MPIWinFence == waitSymbol && otherWaitSymbol == waitSymbol)
              || (MPISymbol::MPIWinTest == waitSymbol && MPISymbol::MPIWinComplete == otherWaitSymbol)
              || (MPISymbol::MPIWinWait == waitSymbol && MPISymbol::MPIWinComplete == otherWaitSymbol)
            ) {
              otherInstructionIdx += 1;
              break;
            }
          }
        }

        if (otherLastStartIdx == otherInstructionIdx) {
          break;
        }

        for (uint64_t i = lastStartIdx; i < instructionIdx; ++i) {
          auto &instruction = guide.instructions[i];
          auto epoch = instruction.epoch;
          bool skip = false;

          switch (instruction.instruction.symbol) {
            case MPISymbol::MPISharedRead:
            case MPISymbol::MPISharedWrite:
            case MPISymbol::MPIAccumulate:
            case MPISymbol::MPICompareAndSwap:
            case MPISymbol::MPIFetchAndOp:
            case MPISymbol::MPIGetAccumulate:
            case MPISymbol::MPIGet:
            case MPISymbol::MPIPut:
            case MPISymbol::MPIRaccumulate:
            case MPISymbol::MPIRget:
            case MPISymbol::MPIRgetAccumulate:
            case MPISymbol::MPIRput:
              break;
            default:
              skip = true;
              break;
          }

          if (skip) {
            continue;
          }

          /**
           * If A is passive/exposing, it won't reach out to anything.
           */
          if ( Epoch::Passive != epoch
            && Epoch::Expose != epoch
          ) {
            for (uint64_t j = otherLastStartIdx; j < otherInstructionIdx; ++j) {
              auto &otherInstruction = otherGuide.instructions[j];
              auto otherEpoch = otherInstruction.epoch;

              if ( (Epoch::Locking == epoch && Epoch::Passive == otherEpoch)
                || (Epoch::Locking == epoch && Epoch::Fence == otherEpoch)
                || (Epoch::LockingAll == epoch && Epoch::Passive == otherEpoch)
                || (Epoch::LockingAll == epoch && Epoch::Fence == otherEpoch)
                || (Epoch::Access == epoch && Epoch::Expose == otherEpoch)
                || (Epoch::Access == epoch && Epoch::ExposeAccess == otherEpoch)
                || (Epoch::Fence == epoch)
              ) {
                /**
                 * Do not take those matches into account that make no sense, as
                 * they cannot be two different things at that time, e.g. by
                 *
                 * if (0 == rank) {
                 *   if (someCond) {
                 *     // Access -> one guide
                 *   } else {
                 *     // Expose -> another guide
                 *   }
                 * }
                 */

                if (!(
                      llvm::CmpInst::Predicate::ICMP_EQ == guide.rankPredicate
                  &&  guide.rankPredicate == otherGuide.rankPredicate
                  &&  guide.rankValue == otherGuide.rankValue
                )) {
                  instruction.matchingInstructions[otherGuide.window].emplace_back(otherInstruction.instruction);
                }
              }
            }
          }
        }
      }
    }
  }

  /**
   * From here, we are only interested in actual communication calls.
   *
   * Warning: shared memory IO cannot likely be handled in terms
   * of destination rank analysis without some rewrite.
   */
  for (auto &pair : context.guides) {
    auto &guide = pair.second;

    for (uint64_t i = guide.instructions.size() - 1; i > 0; --i) {
      bool deleteInstruction = false;
      auto &instruction = guide.instructions[i-1];

      switch (instruction.instruction.symbol) {
        case MPISymbol::MPISharedRead:
        case MPISymbol::MPISharedWrite:
        case MPISymbol::MPIAccumulate:
        case MPISymbol::MPICompareAndSwap:
        case MPISymbol::MPIFetchAndOp:
        case MPISymbol::MPIGetAccumulate:
        case MPISymbol::MPIGet:
        case MPISymbol::MPIPut:
        case MPISymbol::MPIRaccumulate:
        case MPISymbol::MPIRget:
        case MPISymbol::MPIRgetAccumulate:
        case MPISymbol::MPIRput:
          break;
        default:
          deleteInstruction = true;
          break;
      }

      if (deleteInstruction) {
        guide.instructions.erase(guide.instructions.begin() + i - 1);
      }
    }
  }
}

MPISymbol EpochMatcher::getBlockingInstruction (EpochMatcher::Instruction &instruction) const {
  switch (instruction.symbol) {
    case MPISymbol::MPIBarrier:
      if (auto callInst = llvm::dyn_cast<llvm::CallInst>(instruction.instruction)) {
        llvm::ImmutableCallSite callSite(callInst);
        auto barrierValue = callSite.getArgument(0);

        if (auto constant = llvm::dyn_cast<llvm::ConstantInt>(barrierValue)) {
          return instruction.symbol;
        }
      }

      return MPISymbol::Null;
    case MPISymbol::MPIWinComplete:
    case MPISymbol::MPIWinFence:
    case MPISymbol::MPIWinFree:
    case MPISymbol::MPIWinWait:
      return instruction.symbol;
    default:
      return MPISymbol::Null;
  }
}

void EpochMatcher::linearize (Context &context) const {
  for (auto fact : facts) {
    auto path = fact->getPath();
    auto insertionPair = context.guides.emplace(
        std::piecewise_construct
      , std::forward_as_tuple(fact)
      , std::forward_as_tuple(fact, path)
    );

    auto &currentGuide = insertionPair.first->second;
    const WindowPath *currentPath = path;
    uint64_t locks = 0;

    std::stack<const WindowPath*> paths;
    uint64_t totalInstructions = path->getTransitions().size();

    while (nullptr != currentPath) {
      paths.push(currentPath);
      totalInstructions += currentPath->getTransitions().size();
      currentPath = currentPath->getPreviousPath();
    }

    currentGuide.instructions.resize(totalInstructions);

    uint64_t i = 0;
    while (paths.size() > 0) {
      auto path = paths.top();
      paths.pop();

      auto &transitions = path->getTransitions();

      for (auto it = transitions.begin(); it != transitions.end(); ++it, ++i) {
        auto symbol = it->symbol;
        auto epoch = Epoch::Passive;
        auto previousEpoch = Epoch::Passive;

        if (i > 0) {
          previousEpoch = currentGuide.instructions[i-1].epoch;
        }

        switch (symbol) {
          case MPISymbol::MPIWinComplete:
            if ( i > 0
              && Epoch::ExposeAccess == previousEpoch) {
              epoch = Epoch::Expose;
            } else {
              epoch = Epoch::Passive;
            }
            break;
          case MPISymbol::MPIWinFence:
            epoch = Epoch::Fence;
            break;
          case MPISymbol::MPIWinLock:
            epoch = Epoch::Locking;
            break;
          case MPISymbol::MPIWinLockAll:
            epoch = Epoch::LockingAll;
            break;
          case MPISymbol::MPIWinPost:
            epoch = Epoch::Expose;
            break;
          case MPISymbol::MPIWinStart:
            epoch = Epoch::Access;
            break;
          case MPISymbol::MPIWinFree:
          case MPISymbol::MPIWinUnlockAll:
            epoch = Epoch::Passive;
            break;
          case MPISymbol::MPIWinTest:
          case MPISymbol::MPIWinWait:
            if ( i > 0
              && Epoch::ExposeAccess == previousEpoch) {
              epoch = Epoch::Access;
            } else {
              epoch = Epoch::Passive;
            }
            break;
          default:
            epoch = previousEpoch;
            break;
        }

        if (MPISymbol::MPIWinUnlock == symbol) {
          locks -= 1;

          if (0 == locks) {
            epoch = Epoch::Passive;
          }
        } else if (MPISymbol::MPIWinLock == symbol) {
          locks += 1;
        }

        currentGuide.instructions.emplace(
            currentGuide.instructions.begin() + i
          , it->instruction
          , symbol
          , epoch
          , it->generator
        );
      }
    }
  }
}

EpochMatcher::WindowGuides EpochMatcher::match () const {
  EpochMatcher::Context context;

  linearize(context);
  coExecute(context);
  validateMatching(context);

  return context.guides;
}

void EpochMatcher::validateMatching (Context &context) const {
  for (auto &guidePair : context.guides) {
    auto &guide = guidePair.second;

    for (auto &instruction : guide.instructions) {
      // Window-wide modes must have counterparts everywhere
      if ( EpochMatcher::Epoch::LockingAll == instruction.epoch) {
        if (instruction.matchingInstructions.size() != (context.guides.size() - 1)){
          guide.violatingInstruction = instruction.instruction.instruction;
          guide.violation = EpochMatcher::Violation::NotAllAvailable;
          break;
        }
      // Access/Locking must have at least one counterpart
      } else if (
           EpochMatcher::Epoch::Access == instruction.epoch
        || EpochMatcher::Epoch::Locking == instruction.epoch
      ) {
        if (0 == instruction.matchingInstructions.size()) {
          guide.violatingInstruction = instruction.instruction.instruction;
          guide.violation = EpochMatcher::Violation::Unmatched;
          break;
        }
      }
    }
  }
}
