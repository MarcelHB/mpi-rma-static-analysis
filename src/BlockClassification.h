#ifndef H_BLOCK_CLASSIFICATION
#define H_BLOCK_CLASSIFICATION

#include "llvm/IR/BasicBlock.h"

#include "Settings.h"

bool shouldPreserveBlock (const llvm::BasicBlock&, const Settings&);

#endif
