#ifndef H_LAZY_BRANCH_PIPE
#define H_LAZY_BRANCH_PIPE

#include "LazyFact.h"
#include "LazyMPIRMAUseProblem.h"

struct NonSingletonIdentity : public psr::FlowFunction<LazyFact*> {
  std::set<LazyFact*> computeTargets (LazyFact *LazyFact) {
    return {LazyFact};
  }
};

template <typename FF>
struct LazyBranchPipe : public psr::FlowFunction<LazyFact*> {
  const llvm::Instruction *instruction;
  const llvm::Instruction *succInstruction;
  LazyMPIRMAUseProblem &problemInstance;
  FF flowFunction;

  template <typename ... Args>
  LazyBranchPipe (
      const llvm::Instruction *Instruction
    , const llvm::Instruction *SuccInstruction
    , LazyMPIRMAUseProblem &problem
    , Args&& ... args
  ) : instruction(Instruction)
    , succInstruction(SuccInstruction)
    , problemInstance(problem)
    , flowFunction(FF{std::forward<Args>(args) ...})
  {};

  std::set<LazyFact*> computeTargets (LazyFact *factUnit) {
    if (factUnit->isType(LazyFact::Type::Win)) {
      if (nullptr != succInstruction) {
        if (auto branchInst = llvm::dyn_cast<llvm::BranchInst>(succInstruction)) {
          UnlockMapOperation::UnlockMapType unlockMapType =
            UnlockMapOperation::UnlockMapType::Forward;

          if ( branchInst->isConditional()
            && problemInstance.isLoopHead(branchInst, &unlockMapType)
            && !factUnit->awaitsLockAfter(succInstruction)
          ) {
            factUnit->markForPurging(instruction);

            LazyFact::ForkActionType type = LazyFact::ForkActionType::PrepareLoop;
            if (UnlockMapOperation::UnlockMapType::ForwardInDo == unlockMapType) {
              type = LazyFact::ForkActionType::PrepareDoLoop;
            }

            LazyFact* newFact = problemInstance.makeInitialFact(
                *factUnit
              , branchInst
              , type
            );

            return flowFunction.computeTargets(newFact);
          }
        }

        auto nextAction = factUnit->getForkAction(succInstruction);

        switch (nextAction.actionType) {
          case LazyFact::ForkActionType::Lock:
            // Simply discard on any lock indication.
            return {};
          case LazyFact::ForkActionType::ExitLoop: {
            factUnit->markForPurging(succInstruction);

            LazyFact* newFact = problemInstance.makeInitialFact(
                *factUnit
              , succInstruction
              , LazyFact::ForkActionType::ExitLoop
            );

            return flowFunction.computeTargets(newFact);
          }
          default:
            break;
        }
      }
    }

    return flowFunction.computeTargets(factUnit);
  }
};

#endif
