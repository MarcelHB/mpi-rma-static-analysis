#include "ConcurrentCommCheck.h"
#include "RankContradictionResolver.h"

ConcurrentCommCheck::ConcurrentCommCheck (
    EpochMatcher::WindowGuides &Guides
) : guides(Guides)
{}

unsigned ConcurrentCommCheck::getRankArgPosition (MPISymbol symbol) {
  switch (symbol) {
    case MPISymbol::MPIRgetAccumulate:
    case MPISymbol::MPIGetAccumulate:
      return 6;
    case MPISymbol::MPICompareAndSwap:
      return 4;
    case MPISymbol::MPIAccumulate:
    case MPISymbol::MPIFetchAndOp:
    case MPISymbol::MPIGet:
    case MPISymbol::MPIPut:
    case MPISymbol::MPIRget:
    case MPISymbol::MPIRput:
    case MPISymbol::MPIRaccumulate:
      return 3;
    case MPISymbol::MPISharedRead:
    case MPISymbol::MPISharedWrite:
      return 1;
    default:
      return 0;
  }
}

int64_t ConcurrentCommCheck::getRankValue (
    Fact *window
  , const EpochMatcher::MatchedInstruction &instruction
) const {
  const llvm::CallInst *callInst = nullptr;

  if ( instruction.instruction.symbol == MPISymbol::MPISharedRead
    || instruction.instruction.symbol == MPISymbol::MPISharedWrite
  ) {
    callInst = instruction.instruction.generator;
  } else {
    callInst = llvm::dyn_cast<llvm::CallInst>(instruction.instruction.instruction);
  }

  if (nullptr == callInst) {
    return -1;
  }

  auto rankValue = callInst->getArgOperand(getRankArgPosition(instruction.instruction.symbol));
  if (auto constant = llvm::dyn_cast<llvm::ConstantInt>(rankValue)) {
    return constant->getSExtValue();
  } else {
    auto integerList = window->getIntegerSnapshots(callInst);

    if (nullptr == integerList) {
      return -1;
    }

    bool skip = false;
    std::set<int64_t> candidateValues{};

    for (auto &item : *integerList) {
      if (item.fact->knowsValue(rankValue)) {
        if (item.isLost) {
          skip = true;
          break;
        } else {
          candidateValues.insert(item.value);
        }
      }
    }

    if (skip || candidateValues.size() != 1) {
      return -1;
    }

    return *(candidateValues.begin());
  }
}

std::set<Fact*> ConcurrentCommCheck::getTargetWindows (
    int64_t rankConstantValue
  , const EpochMatcher::MatchedInstruction &instruction
) const {
  std::set<Fact*> targetWindows{};

  for (auto &otherPair : instruction.matchingInstructions) {
    auto otherWindow = otherPair.first;
    auto path = otherWindow->getPath();

    if (!path->hasRankPredicate()) {
      continue;
    }

    auto otherRankPredicate = path->getRankPredicate();
    auto otherRankValue = path->getRankValue();

    if ( llvm::CmpInst::Predicate::ICMP_EQ == otherRankPredicate
      && otherRankValue == rankConstantValue
    ) {
      targetWindows.insert(otherWindow);
    }
  }

  // If there is just one other code path, take that one.
  // Idea: Grouping multiple windows of same epoch code into one. But efficiently.
  if (targetWindows.size() == 0 && instruction.matchingInstructions.size() == 1) {
    targetWindows.insert(instruction.matchingInstructions.begin()->first);
  }

  // Try anything that is not a contradiction.
  if (targetWindows.size() == 0) {
    auto condA = RankContradictionResolver::ResolvedCondition{
        nullptr
      , std::move(
          RankContradictionResolver::Condition{llvm::CmpInst::Predicate::ICMP_EQ, rankConstantValue}
        )
    };

    for (auto &otherPair : instruction.matchingInstructions) {
      auto otherWindow = otherPair.first;
      auto path = otherWindow->getPath();

      if (!path->hasRankPredicate()) {
        continue;
      }

      auto otherRankPredicate = path->getRankPredicate();
      auto otherRankValue = path->getRankValue();

      auto condB = RankContradictionResolver::ResolvedCondition{
          nullptr
        , std::move(
            RankContradictionResolver::Condition{otherRankPredicate, otherRankValue}
          )
      };

      RankContradictionResolver resolver{condA, condB};
      if (!resolver.isContradiction()) {
        targetWindows.insert(otherWindow);
      }
    }
  }

  return targetWindows;
}

std::map<Fact*, const llvm::Instruction*> ConcurrentCommCheck::validate () const {
  std::map<Fact*, const llvm::Instruction*> violations;

  for (auto &pair : guides) {
    auto &guide = pair.second;
    auto window = pair.first;

    for (auto &instruction : guide.instructions) {
      if (nullptr == instruction.instruction.instruction) {
        continue;
      }

      int64_t rankConstantValue = getRankValue(window, instruction);
      if (rankConstantValue < 0) {
        continue;
      }

      auto targets = getTargetWindows(rankConstantValue, instruction);

      if (0 == targets.size()) {
        continue;
      }

      for (auto target : targets) {
        for (auto &otherInstruction : instruction.matchingInstructions[target]) {
          if (MPISymbol::MPILocalWrite == otherInstruction.symbol) {
            violations[window] = otherInstruction.instruction;
            break;
          }
        }
      }
    }
  }

  return violations;
}

