#ifndef H_SWITCH_CONVERTER
#define H_SWITCH_CONVERTER

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

/**
 * As some ideas were older than facing the problem of `switch` statements
 * of LLVM, some algorithms rely on having no more than two successor BBs.
 *
 * So, instead rethinking the algorithms, just unroll `switches` into whatever
 * many BBs chained over regular conditional branches.
 */

class SwitchConverter {
  public:
    SwitchConverter (llvm::Module&, llvm::LLVMContext&);

    void rewrite ();
  private:
    llvm::Module &module;
    llvm::LLVMContext &context;

    std::vector<llvm::BasicBlock*> getBlocksToRewrite (llvm::Function&);
    void rewriteBlocks (std::vector<llvm::BasicBlock*>&, llvm::Function&);
};

#endif
