#include "llvm/IR/Instructions.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

#include "GraphSlicer.h"

GraphSlicer::GraphSlicer (const WormholeMapGenerator::OperationWormholeMap &Map)
  : wormholeMap(Map)
{}

void GraphSlicer::slice () const {
  for (auto &pair : wormholeMap) {
    auto &entry = pair.second;
    if (auto terminator = llvm::dyn_cast<llvm::BranchInst>(pair.first->getTerminator())) {
      if (entry.leftRight) {
        if (nullptr != entry.left) {
          terminator->setSuccessor(0, entry.left);
        } else {
          terminator->setSuccessor(1, entry.right);
        }
      } else {
        auto branchInst = llvm::BranchInst::Create(entry.left);
        llvm::ReplaceInstWithInst(terminator, branchInst);
      }
    }
  }
}
