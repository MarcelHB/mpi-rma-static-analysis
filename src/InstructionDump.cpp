#include <sstream>

#include "llvm/IR/DebugInfoMetadata.h"

#include "InstructionDump.h"

std::string dumpInstruction (const llvm::Instruction *Instruction) {
  if (Instruction->getMetadata(llvm::LLVMContext::MD_dbg)) {
    auto &debugLoc = Instruction->getDebugLoc();

    std::stringstream sstream;

    if (auto scope = debugLoc->getScope()) {
      sstream << scope->getFilename().str() << ":";
      sstream << debugLoc.getLine() << ": ";
      sstream << scope->getSubprogram()->getName().str();

      return sstream.str();
    }
  }

  return std::string{"n/a"};
}
