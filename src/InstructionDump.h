#ifndef H_INSTRUCTION_DUMP
#define H_INSTRUCTION_DUMP

#include <string>

#include "llvm/IR/Instruction.h"

std::string dumpInstruction (const llvm::Instruction*);

#endif
