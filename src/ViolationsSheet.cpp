#include "ViolationsSheet.h"

#include "phasar/Utils/LLVMShorthands.h"

#include "InstructionDump.h"

ViolationsSheet::ViolationsSheet (std::ostream& OutStream)
  : outStream(OutStream)
{}

void ViolationsSheet::writeHeaderRow () {
  outStream << "win_id" << ViolationsSheet::colSep
    << "violation" << ViolationsSheet::colSep
    << "code_location" << ViolationsSheet::colSep
    << "llvm_location"
    << std::endl;
}

void ViolationsSheet::writeViolation (
    const Fact *Window
  , const llvm::Instruction *Instruction
  , const std::string &Name
) {
  outStream << Window->getId() << ViolationsSheet::colSep
    << Name << ViolationsSheet::colSep
    << dumpInstruction(Instruction) << ViolationsSheet::colSep
    << "\"" << psr::llvmIRToString(Instruction) << "\""
    << std::endl;
}
