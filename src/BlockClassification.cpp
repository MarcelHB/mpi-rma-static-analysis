#include "llvm/IR/CallSite.h"
#include "llvm/IR/Constants.h"

#include "BlockClassification.h"

bool shouldPreserveBlock (
    const llvm::BasicBlock &Block
  , const Settings &S
) {
  for (auto &instruction : Block) {
    if (auto callInst = llvm::dyn_cast<llvm::CallInst>(&instruction)) {
      llvm::ImmutableCallSite callsite{callInst};

      auto targetFunction = callsite.getCalledFunction();

      if (nullptr == targetFunction) {
        auto strippedValue = callsite.getCalledValue()->stripPointerCasts();

        if (auto pointedFunction = llvm::dyn_cast<llvm::Function>(strippedValue)) {
          targetFunction = pointedFunction;
        }
      }

      if (nullptr != targetFunction) {
        if ( targetFunction->isDeclaration()
          && (
               targetFunction->getName().startswith_lower("MPI_Accumulate")
            || targetFunction->getName().startswith_lower("MPI_Barrier")
            || targetFunction->getName().startswith_lower("MPI_Comm_accept")
            || targetFunction->getName().startswith_lower("MPI_Comm_connect")
            || targetFunction->getName().startswith_lower("MPI_Comm_create_group")
            || targetFunction->getName().equals("MPI_Comm_create")
            || targetFunction->getName().equals_lower("MPI_Comm_create_")
            || targetFunction->getName().startswith_lower("MPI_Comm_disconnect")
            || targetFunction->getName().startswith_lower("MPI_Comm_dup")
            || targetFunction->getName().startswith_lower("MPI_Comm_idup")
            || targetFunction->getName().startswith_lower("MPI_Comm_free")
            || targetFunction->getName().startswith_lower("MPI_Comm_join")
            || targetFunction->getName().startswith_lower("MPI_Comm_rank")
            || targetFunction->getName().startswith_lower("MPI_Comm_spawn")
            || targetFunction->getName().startswith_lower("MPI_Comm_split")
            || targetFunction->getName().startswith_lower("MPI_Compare_and_swap")
            || targetFunction->getName().startswith_lower("MPI_Fetch_and_op")
            || targetFunction->getName().startswith_lower("MPI_Finalize")
            || targetFunction->getName().startswith_lower("MPI_Get_accumulate")
            || targetFunction->getName().equals("MPI_Get")
            || targetFunction->getName().equals_lower("MPI_Get_")
            || targetFunction->getName().startswith_lower("MPI_Intercomm_create")
            || targetFunction->getName().startswith_lower("MPI_Intercomm_merge")
            || targetFunction->getName().startswith_lower("MPI_Put")
            || targetFunction->getName().startswith_lower("MPI_Rget")
            || targetFunction->getName().startswith_lower("MPI_Rput")
            || targetFunction->getName().startswith_lower("MPI_Win_allocate")
            || targetFunction->getName().startswith_lower("MPI_Win_attach")
            || targetFunction->getName().startswith_lower("MPI_Win_complete")
            || targetFunction->getName().startswith_lower("MPI_Win_create_dynamic")
            || targetFunction->getName().equals("MPI_Win_create")
            || targetFunction->getName().equals_lower("MPI_Win_create_")
            || targetFunction->getName().startswith_lower("MPI_Win_detach")
            || targetFunction->getName().startswith_lower("MPI_Win_fence")
            || targetFunction->getName().startswith_lower("MPI_Win_flush")
            || targetFunction->getName().equals("MPI_Win_free")
            || targetFunction->getName().equals_lower("MPI_Win_free_")
            || targetFunction->getName().startswith_lower("MPI_Win_lock")
            || targetFunction->getName().startswith_lower("MPI_Win_post")
            || targetFunction->getName().startswith_lower("MPI_Win_shared_query")
            || targetFunction->getName().startswith_lower("MPI_Win_start")
            || targetFunction->getName().startswith_lower("MPI_Win_sync")
            || targetFunction->getName().startswith_lower("MPI_Win_test")
            || targetFunction->getName().startswith_lower("MPI_Win_unlock")
            || targetFunction->getName().startswith_lower("MPI_Win_wait")
          )
        ) {
          return true;
        /**
         * A more advanced approach may recursively scan called definitions (once!),
         * summarize their scan value, and cut away whole in-module dives.
         */
        } else if (!targetFunction->isDeclaration()) {
          return true;
        }
      }
    } else if (auto storeInst = llvm::dyn_cast<llvm::StoreInst>(&instruction)) {
      if (Settings::IntegerMode::None != S.integerMode) {
        if (auto constValue = llvm::dyn_cast<llvm::ConstantInt>(storeInst->getValueOperand())) {
          return true;
        }
      }
    }
  }

  return false;
}
