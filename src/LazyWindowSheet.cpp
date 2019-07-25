#include <sstream>
#include <stack>

#include "llvm/IR/CallSite.h"
#include "phasar/Utils/LLVMShorthands.h"

#include "InstructionDump.h"
#include "LazyWindowSheet.h"

LazyWindowSheet::LazyWindowSheet (std::ostream &OutStream)
  : outStream(OutStream)
{}

void LazyWindowSheet::writeHeaderRow () {
  outStream << "win_id" << LazyWindowSheet::colSep
    << "win_state" << LazyWindowSheet::colSep
    << "rank_pred" << LazyWindowSheet::colSep
    << "rank_value" << LazyWindowSheet::colSep
    << "code_location" << LazyWindowSheet::colSep
    << "llvm_location"
    << std::endl;
}

void LazyWindowSheet::writeWindowFact (const LazyFact *Window) {
  std::string windowState{"valid"};

  std::stack<const LazyFact*> pathSegments;
  const LazyFact *currentSegment = Window;

  while (nullptr != currentSegment) {
    pathSegments.push(currentSegment);
    currentSegment = currentSegment->getPreviousFact();
  }

  while (pathSegments.size() > 0) {
    currentSegment = pathSegments.top();
    pathSegments.pop();

    auto &SM = Window->getState();
    auto instruction = currentSegment->getCreatingInstruction();

    if (Window->isMarkedForViolation()) {
      windowState.assign("violation");
    } else if (!SM.hasFinalState()) {
      windowState.assign("unfinished");
    }

    outStream << Window->getId() << LazyWindowSheet::colSep
      << windowState << LazyWindowSheet::colSep
      << "N/A" << LazyWindowSheet::colSep
      << "N/A" << LazyWindowSheet::colSep
      << dumpInstruction(instruction) << LazyWindowSheet::colSep
      << "\"" << psr::llvmIRToString(instruction) << "\""
      << std::endl;
  }
}

