#include <stack>

#include "RankAttributionAndContradictionCheck.h"
#include "RankContradictionResolver.h"
#include "WindowPath.h"

RankAttributionAndContradictionCheck::RankAttributionAndContradictionCheck (
    Fact *Window
) : window(Window)
{}

const llvm::Instruction* RankAttributionAndContradictionCheck::attributeAndValidate () {
  std::stack<const WindowPath*> paths;
  const WindowPath* currentPath = window->getPath();

  while (nullptr != currentPath) {
    paths.push(currentPath);
    currentPath = currentPath->getPreviousPath();
  }

  RankContradictionResolver::ResolvedCondition firstCondition{};

  while (paths.size() > 0) {
    auto path = paths.top();
    paths.pop();

    auto &transitions = path->getTransitions();

    for (auto it = transitions.begin(); it != transitions.end(); ++it) {
      auto stateSymbol = it->state;

      if ( WindowState::State::ForkTrue != stateSymbol
        && WindowState::State::ForkFalse != stateSymbol
      ) {
        continue;
      }

      if (nullptr == firstCondition.instruction) {
        auto firstInst = llvm::dyn_cast<llvm::BranchInst>(it->instruction);
        auto cmpInst = llvm::dyn_cast<llvm::ICmpInst>(firstInst->getCondition());

        if (nullptr == cmpInst) {
          continue;
        }

        const llvm::Value *valueArg = getRankValueFromCondition(window, cmpInst);
        if (nullptr == valueArg) {
          continue;
        }

        auto integerList = window->getIntegerSnapshots(cmpInst);

        firstCondition = RankContradictionResolver::ResolvedCondition{
            cmpInst
          , std::move(
              RankContradictionResolver::getPositiveCondition(
                  cmpInst
                , valueArg
                , integerList
                , stateSymbol
              )
            )
        };

        if (llvm::CmpInst::Predicate::BAD_ICMP_PREDICATE == firstCondition.condition.pred) {
          firstCondition.instruction = nullptr;
        }
      } else {
        auto otherInst = llvm::dyn_cast<llvm::BranchInst>(it->instruction);
        auto otherCmpInst = llvm::dyn_cast<llvm::ICmpInst>(otherInst->getCondition());

        if (nullptr == otherCmpInst) {
          continue;
        }

        const llvm::Value *valueArg = getRankValueFromCondition(window, otherCmpInst);
        if (nullptr == valueArg) {
          continue;
        }

        auto integerList = window->getIntegerSnapshots(otherCmpInst);

        auto otherCondition = RankContradictionResolver::ResolvedCondition{
            otherCmpInst
          , std::move(
              RankContradictionResolver::getPositiveCondition(
                  otherCmpInst
                , valueArg
                , integerList
                , stateSymbol
              )
            )
        };

        if (llvm::CmpInst::Predicate::BAD_ICMP_PREDICATE == otherCondition.condition.pred) {
          continue;
        }

        RankContradictionResolver resolver{
            firstCondition
          , otherCondition
        };

        if (resolver.isContradiction()) {
          return otherInst;
        }

        auto specificCondition = resolver.getMoreSpecificCondition();

        if (firstCondition.instruction != specificCondition.instruction) {
          firstCondition = specificCondition;
        }
      }
    }
  }

  if (nullptr != firstCondition.instruction) {
    auto windowPath = window->getPath();
    windowPath->setRankAttributes(
        firstCondition.condition.pred
      , firstCondition.condition.constantValue
    );
  }

  return nullptr;
}

const llvm::Value* RankAttributionAndContradictionCheck::getRankValueFromCondition (
    const Fact *window
  , const llvm::CmpInst *inst
) const {
  const Fact *generator = window->getGeneratorUnit();

  if (nullptr == generator) {
    generator = window->getZeroUnit();
  }

  const llvm::Value *firstArg = inst->getOperand(0);
  const llvm::Value *secondArg = inst->getOperand(1);

  if (generator->knowsRankValue(firstArg)) {
    return secondArg;
  } else if (generator->knowsRankValue(secondArg)) {
    return firstArg;
  }

  return nullptr;
}
