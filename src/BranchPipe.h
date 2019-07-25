#ifndef H_BRANCH_PIPE
#define H_BRANCH_PIPE

#ifdef WITH_SIGNAL
#include <signal.h>
#endif

#include "phasar/PhasarLLVM/IfdsIde/FlowFunction.h"

#include "Fact.h"
#include "MPIRMAUseProblem.h"

/**
 * Just acting like the default Phasar identify flow function,
 * but since it's not accessible as a singleton exclusively, we
 * can merge this more easily into the branch pipe below.
 */
struct NonSingletonIdentity : public psr::FlowFunction<Fact*> {
  std::set<Fact*> computeTargets (Fact *Fact) {
    return {Fact};
  }
};

template <typename FF>
struct BranchPipe : public psr::FlowFunction<Fact*> {
  const llvm::Instruction *instruction;
  const llvm::Function *targetFunction;
  MPIRMAUseProblem &problemInstance;
  FF flowFunction;

  template <typename ... Args>
  BranchPipe (
      const llvm::Instruction *Instruction
    , MPIRMAUseProblem &problem
    , Args&& ... args
  ) : instruction(Instruction)
    , targetFunction(nullptr)
    , problemInstance(problem)
    , flowFunction(FF{std::forward<Args>(args) ...})
  {};

  template <typename ... Args>
  BranchPipe (
      const llvm::Instruction *Instruction
    , const llvm::Function *TargetFn
    , MPIRMAUseProblem &problem
    , Args&& ... args
  ) : instruction(Instruction)
    , targetFunction(TargetFn)
    , problemInstance(problem)
    , flowFunction(FF{std::forward<Args>(args) ...})
  {};

  std::set<Fact*> computeTargets (Fact *factUnit) {
    if (factUnit->isType(Fact::Type::IntConst)) {
      bool doSnapshot = false;

      // Invalidate integers if they have been passed as pointer to some ext. call
      if (auto callInstruction = llvm::dyn_cast<llvm::CallInst>(instruction)) {
        if (nullptr != targetFunction && targetFunction->isDeclaration()) {
          doSnapshot = true;

          for (unsigned idx = 0; idx < callInstruction->getNumArgOperands(); ++idx) {
            auto operand = callInstruction->getArgOperand(idx);

            if (  factUnit->knowsValue(operand)
              &&  operand->getType()->isPointerTy()
            ) {
              factUnit->markIntegerUnresolvable();
            }
          }
        }
      } else if (auto compareInst = llvm::dyn_cast<llvm::CmpInst>(instruction)) {
        doSnapshot = true;
      }

      /**
       * Reduce snapshots of integers to instructions where we really may
       * need to look them up at a later point, that are:
       *
       * * Calls to declarations, as MPI calls are.
       * * Comparisons, as we look for rank values there.
       */
      if (doSnapshot) {
        factUnit->snapshotIntegerValue(instruction);
      }
    }

    if (!factUnit->isStillForkable(instruction)) {
      return flowFunction.computeTargets(factUnit);
    }

    if (  factUnit->isType(Fact::Type::Win)
      ||  factUnit->isType(Fact::Type::IntConst)
    ) {
      auto nextAction = factUnit->getForkAction(instruction);

      switch (nextAction.actionType) {
        case Fact::ForkActionType::ForkTrue:
        case Fact::ForkActionType::ForkFalse: {
          factUnit->markForPurging(instruction);
          Fact* newFact = problemInstance.makeInitialFact(*factUnit, instruction);

          if (factUnit->isType(Fact::Type::Win)) {
#ifdef WITH_SIGNAL
            raise(SIGUSR1);
#endif
            if (nextAction.actionType == Fact::ForkActionType::ForkTrue) {
              newFact->markAsForkedByCondition(factUnit->getBranchMarker(), true);
            } else if (nextAction.actionType == Fact::ForkActionType::ForkFalse) {
              newFact->markAsForkedByCondition(factUnit->getBranchMarker(), false);
            }
          }

          return flowFunction.computeTargets(newFact);
        }
        case Fact::ForkActionType::Drop:
          factUnit->markForPurging();

          return {};
        case Fact::ForkActionType::Lock:
          factUnit->lock(nextAction.instruction, instruction);
          break;
        case Fact::ForkActionType::Unlock: {
          factUnit->markForPurging(instruction);
          Fact* newFact = problemInstance.makeInitialFact(*factUnit, instruction);

          return flowFunction.computeTargets(newFact);
        }
        default:
          break;
      }
    }

    return flowFunction.computeTargets(factUnit);
  }
};

#endif
