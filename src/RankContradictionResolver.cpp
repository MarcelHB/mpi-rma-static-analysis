#include <set>

#include "RankContradictionResolver.h"

RankContradictionResolver::RankContradictionResolver (
    const RankContradictionResolver::ResolvedCondition &Condition
  , const RankContradictionResolver::ResolvedCondition &OtherCondition
) : condition(Condition)
  , otherCondition(OtherCondition)
{}

using LLVMPred = llvm::CmpInst::Predicate;

/**
 * Return the preset condition if:
 * * conditions are equivalent,
 * * conditions can't be resolved into a more specific version,
 * * it's more specific of course....
 *
 * Return the other condition if:
 * * it's more specific,
 * * we have synthesized a better condition.
 *
 * A `nullptr` containing struct is returned on conflicts.
 */
const RankContradictionResolver::ResolvedCondition
RankContradictionResolver::getMoreSpecificCondition () const {
  if (isContradiction()) {
    return RankContradictionResolver::ResolvedCondition{};
  }

  auto &condA = condition.condition;
  auto &condB = otherCondition.condition;

  auto &condAResult = condition;
  auto &condBResult = otherCondition;

  if (condA.pred == condB.pred && condA.constantValue == condB.constantValue) {
    return condAResult;
  }

  // EQ > everything else
  if (LLVMPred::ICMP_EQ == condA.pred) {
    return condAResult;
  } else if (LLVMPred::ICMP_EQ == condB.pred) {
    return condBResult;
  }

  // Same constant, ULT > ULE; UGT > UGE
  if (condA.constantValue == condB.constantValue) {
    if ( (condA.pred == LLVMPred::ICMP_ULT && condB.pred == LLVMPred::ICMP_ULE)
      || (condA.pred == LLVMPred::ICMP_UGT && condB.pred == LLVMPred::ICMP_UGE)
    ) {
      return condAResult;
    } else if (
         (condB.pred == LLVMPred::ICMP_ULT && condA.pred == LLVMPred::ICMP_ULE)
      || (condB.pred == LLVMPred::ICMP_UGT && condA.pred == LLVMPred::ICMP_UGE)
    ) {
      return condBResult;
    // Merge <= x, >= x into == x
    } else if (
         (condA.pred == LLVMPred::ICMP_ULE && condB.pred == LLVMPred::ICMP_UGE)
      || (condA.pred == LLVMPred::ICMP_UGE && condB.pred == LLVMPred::ICMP_ULE)
    ) {
      return RankContradictionResolver::ResolvedCondition{
          otherCondition.instruction
        , std::move(Condition{LLVMPred::ICMP_EQ, condA.constantValue})
      };
    }
  }

  return condAResult;
}

bool RankContradictionResolver::isContradiction () const {
  auto &condA = condition.condition;
  auto &condB = otherCondition.condition;

  // Same predicate, same constant
  if (condA.pred == condB.pred && condA.constantValue == condB.constantValue) {
    return false;
  }

  // Both equal, different constants
  if ( LLVMPred::ICMP_EQ == condA.pred
    && condA.pred == condB.pred
    && condA.constantValue != condB.constantValue
  ) {
    return true;
  }

  // Both not equal, different constants
  if ( LLVMPred::ICMP_NE == condA.pred
    && condA.pred == condB.pred
    && condA.constantValue != condB.constantValue
  ) {
    return false;
  }

  // A < B
  if (  condA.constantValue < condB.constantValue
    && (
         (condA.pred == LLVMPred::ICMP_ULT && condB.pred == LLVMPred::ICMP_UGT)
      || (condA.pred == LLVMPred::ICMP_ULE && condB.pred == LLVMPred::ICMP_UGT)
      || (condA.pred == LLVMPred::ICMP_ULT && condB.pred == LLVMPred::ICMP_UGE)
      || (condA.pred == LLVMPred::ICMP_ULE && condB.pred == LLVMPred::ICMP_UGE)
      || (condA.pred == LLVMPred::ICMP_ULT && condB.pred == LLVMPred::ICMP_EQ)
      || (condA.pred == LLVMPred::ICMP_ULE && condB.pred == LLVMPred::ICMP_EQ)
      || (condA.pred == LLVMPred::ICMP_EQ  && condB.pred == LLVMPred::ICMP_UGT)
      || (condA.pred == LLVMPred::ICMP_EQ  && condB.pred == LLVMPred::ICMP_UGE)
    )
  ) {
    return true;
  }

  // A <= B
  if (  condA.constantValue <= condB.constantValue
    && (
         (condA.pred == LLVMPred::ICMP_EQ  && condB.pred == LLVMPred::ICMP_UGT)
      || (condA.pred == LLVMPred::ICMP_ULE && condB.pred == LLVMPred::ICMP_UGT)
      || (condA.pred == LLVMPred::ICMP_ULT && condB.pred == LLVMPred::ICMP_UGT)
    )
  ) {
    return true;
  }

  // A == B
  if (  condA.constantValue == condB.constantValue
    && (
         (condA.pred == LLVMPred::ICMP_UGT && condB.pred == LLVMPred::ICMP_ULE)
      || (condA.pred == LLVMPred::ICMP_UGT && condB.pred == LLVMPred::ICMP_ULT)
      || (condA.pred == LLVMPred::ICMP_UGT && condB.pred == LLVMPred::ICMP_EQ)
      || (condA.pred == LLVMPred::ICMP_UGE && condB.pred == LLVMPred::ICMP_ULT)
      || (condA.pred == LLVMPred::ICMP_ULT && condB.pred == LLVMPred::ICMP_UGE)
      || (condA.pred == LLVMPred::ICMP_ULT && condB.pred == LLVMPred::ICMP_UGT)
      || (condA.pred == LLVMPred::ICMP_ULT && condB.pred == LLVMPred::ICMP_EQ)
      || (condA.pred == LLVMPred::ICMP_ULE && condB.pred == LLVMPred::ICMP_UGT)
      || (condA.pred == LLVMPred::ICMP_EQ  && condB.pred == LLVMPred::ICMP_NE)
      || (condB.pred == LLVMPred::ICMP_UGT && condA.pred == LLVMPred::ICMP_ULE)
      || (condB.pred == LLVMPred::ICMP_UGT && condA.pred == LLVMPred::ICMP_ULT)
      || (condB.pred == LLVMPred::ICMP_UGT && condA.pred == LLVMPred::ICMP_EQ)
      || (condB.pred == LLVMPred::ICMP_UGE && condA.pred == LLVMPred::ICMP_ULT)
      || (condB.pred == LLVMPred::ICMP_ULT && condA.pred == LLVMPred::ICMP_UGE)
      || (condB.pred == LLVMPred::ICMP_ULT && condA.pred == LLVMPred::ICMP_UGT)
      || (condB.pred == LLVMPred::ICMP_ULT && condA.pred == LLVMPred::ICMP_EQ)
      || (condB.pred == LLVMPred::ICMP_ULE && condA.pred == LLVMPred::ICMP_UGT)
      || (condB.pred == LLVMPred::ICMP_EQ  && condA.pred == LLVMPred::ICMP_NE)
    )
  ) {
    return true;
  }

  // A >= B
  if (  condA.constantValue >= condB.constantValue
    && (
         (condB.pred == LLVMPred::ICMP_EQ  && condA.pred == LLVMPred::ICMP_UGT)
      || (condB.pred == LLVMPred::ICMP_ULE && condA.pred == LLVMPred::ICMP_UGT)
      || (condB.pred == LLVMPred::ICMP_ULT && condA.pred == LLVMPred::ICMP_UGT)
    )
  ) {
    return true;
  }

  // A > B
  if (  condA.constantValue > condB.constantValue
    && (
         (condB.pred == LLVMPred::ICMP_ULT && condA.pred == LLVMPred::ICMP_UGT)
      || (condB.pred == LLVMPred::ICMP_ULE && condA.pred == LLVMPred::ICMP_UGT)
      || (condB.pred == LLVMPred::ICMP_ULT && condA.pred == LLVMPred::ICMP_UGE)
      || (condB.pred == LLVMPred::ICMP_ULE && condA.pred == LLVMPred::ICMP_UGE)
      || (condB.pred == LLVMPred::ICMP_ULT && condA.pred == LLVMPred::ICMP_EQ)
      || (condB.pred == LLVMPred::ICMP_ULE && condA.pred == LLVMPred::ICMP_EQ)
      || (condB.pred == LLVMPred::ICMP_EQ  && condA.pred == LLVMPred::ICMP_UGT)
      || (condB.pred == LLVMPred::ICMP_EQ  && condA.pred == LLVMPred::ICMP_UGE)
    )
  ) {
    return true;
  }

  return false;
}

RankContradictionResolver::Condition RankContradictionResolver::getPositiveCondition (
    const llvm::ICmpInst *Condition
  , const llvm::Value *ValueArg
  , const std::list<Fact::IntegerValueSnapshot> *IntegerValues
  , WindowState::State BranchPath
) {
  int64_t constantValue = -1;
  LLVMPred pred = Condition->getPredicate();

  if (WindowState::State::ForkFalse == BranchPath) {
    pred = Condition->getInversePredicate();
  }

  if (auto constant = llvm::dyn_cast<llvm::ConstantInt>(ValueArg)) {
    constantValue = constant->getSExtValue();
  } else if (nullptr == IntegerValues) {
    pred = llvm::CmpInst::Predicate::BAD_ICMP_PREDICATE;
  } else {
    std::set<int64_t> candidateValues{};

    for (auto &item : *IntegerValues) {
      if (item.fact->knowsValue(ValueArg)) {
        if (item.isLost) {
          pred = llvm::CmpInst::Predicate::BAD_ICMP_PREDICATE;
          break;
        } else {
          candidateValues.insert(item.value);

          if (candidateValues.size() > 1) {
            break;
          }
        }
      }
    }

    if (candidateValues.size() != 1) {
      pred = llvm::CmpInst::Predicate::BAD_ICMP_PREDICATE;
    } else {
      constantValue = *(candidateValues.begin());
    }
  }

  // Exploiting some MPI assumptions such as ranks being >= 0.
  if (0 == constantValue) {
    if ( LLVMPred::ICMP_ULE == pred
      || LLVMPred::ICMP_SLE == pred
    ) {
      pred = LLVMPred::ICMP_EQ;
    } else if (LLVMPred::ICMP_NE == pred) {
      pred = LLVMPred::ICMP_UGT;
    }
  } else if (1 == constantValue) {
    if ( LLVMPred::ICMP_SLT == pred
      || LLVMPred::ICMP_ULT == pred
    ) {
      pred = LLVMPred::ICMP_EQ;
      constantValue = 0;
    }
  }

  // Let's hope we can live with this.
  switch (pred) {
    case LLVMPred::ICMP_SGE:
      pred = LLVMPred::ICMP_UGE;
      break;
    case LLVMPred::ICMP_SGT:
      pred = LLVMPred::ICMP_UGT;
      break;
    case LLVMPred::ICMP_SLE:
      pred = LLVMPred::ICMP_ULE;
      break;
    case LLVMPred::ICMP_SLT:
      pred = LLVMPred::ICMP_ULT;
      break;
    default:
      break;
  }

  return RankContradictionResolver::Condition{pred, constantValue};
}
