#include "llvm/IR/Constants.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

#include "SwitchConverter.h"

SwitchConverter::SwitchConverter (llvm::Module &Module, llvm::LLVMContext &Context)
  : module(Module)
  , context(Context)
{}

void SwitchConverter::rewrite () {
  for (auto &function : module.functions()) {
    auto blocks = getBlocksToRewrite(function);
    rewriteBlocks(blocks, function);
  }
}

std::vector<llvm::BasicBlock*> SwitchConverter::getBlocksToRewrite (llvm::Function &function) {
  std::vector<llvm::BasicBlock*> blocksToRewrite{function.size(), nullptr};

  uint64_t i = 0;
  for (auto &bb : function) {
    auto instruction = bb.getTerminator();

    if (auto switchInst = llvm::dyn_cast<llvm::SwitchInst>(instruction)) {
      blocksToRewrite[i] = &bb;
      i += 1;
    }
  }

  blocksToRewrite.resize(i);

  return blocksToRewrite;
}

void SwitchConverter::rewriteBlocks (std::vector<llvm::BasicBlock*> &blocks, llvm::Function &function) {
  for (auto block : blocks) {
    auto lastInstruction = block->getTerminator();

    llvm::SwitchInst *switchInst = llvm::cast<llvm::SwitchInst>(lastInstruction);
    auto condition = switchInst->getCondition();
    auto lastDestination = switchInst->getDefaultDest();

    for (auto &switchCase : switchInst->cases()) {
      auto newBlock = llvm::BasicBlock::Create(context, llvm::Twine{"switch_unroll"}, &function);
      auto newCondition = new llvm::ICmpInst{*newBlock, llvm::CmpInst::Predicate::ICMP_EQ, condition, switchCase.getCaseValue()};
      llvm::BranchInst::Create(switchCase.getCaseSuccessor(), lastDestination, newCondition, newBlock);
      lastDestination = newBlock;
    }

    auto replaceLink = llvm::BranchInst::Create(lastDestination);

    llvm::ReplaceInstWithInst(switchInst, replaceLink);
  }
}
