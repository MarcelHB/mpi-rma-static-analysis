#include <sstream>
#include <stack>

#include "llvm/IR/CallSite.h"
#include "phasar/Utils/LLVMShorthands.h"

#include "InstructionDump.h"
#include "WindowSheet.h"

WindowSheet::WindowSheet (std::ostream &OutStream)
  : outStream(OutStream)
{}

std::string WindowSheet::getRankPredicateString (llvm::CmpInst::Predicate pred) const {
  switch (pred) {
    case llvm::CmpInst::Predicate::ICMP_EQ:
      return {"=="};
    case llvm::CmpInst::Predicate::ICMP_NE:
      return {"!="};
    case llvm::CmpInst::Predicate::ICMP_ULT:
      return {"<"};
    case llvm::CmpInst::Predicate::ICMP_ULE:
      return {"<="};
    case llvm::CmpInst::Predicate::ICMP_UGT:
      return {">"};
    case llvm::CmpInst::Predicate::ICMP_UGE:
      return {">="};
    default:
      return {"N/A"};
  }

  return {};
}

void WindowSheet::writeHeaderRow () {
  outStream << "win_id" << WindowSheet::colSep
    << "win_state" << WindowSheet::colSep
    << "rank_pred" << WindowSheet::colSep
    << "rank_value" << WindowSheet::colSep
    << "code_location" << WindowSheet::colSep
    << "llvm_location"
    << std::endl;
}

void WindowSheet::writeWindowFact (const Fact *Window) {
  std::string rankPredicate;
  std::string rankValue;
  std::string windowState{"valid"};
  auto path = Window->getPath();

  if (path->hasRankPredicate()) {
    rankPredicate.assign(getRankPredicateString(path->getRankPredicate()));
    std::stringstream ss;
    ss << path->getRankValue();
    rankValue = ss.str();
  }

  if (nullptr != path->getViolatingInstruction()) {
    windowState.assign("violation");
  } else if (!path->hasFinalState()) {
    windowState.assign("unfinished");
  }

  std::stack<const WindowPath*> pathSegments;
  const WindowPath *currentPath = path;

  while (nullptr != currentPath) {
    pathSegments.push(currentPath);
    currentPath = currentPath->getPreviousPath();
  }

  while (pathSegments.size() > 0) {
    currentPath = pathSegments.top();
    pathSegments.pop();

    auto &transitions = currentPath->getTransitions();

    // TODO: escape LLVM output
    for (auto &transition : transitions) {
      auto instruction = transition.instruction;

      if (MPISymbol::MPIBarrier == transition.symbol) {
        if (auto callInst = llvm::dyn_cast<llvm::CallInst>(instruction)) {
          llvm::ImmutableCallSite callSite(callInst);
          auto commValue = callSite.getArgument(0);

          if (!Window->knowsCommunicator(commValue)) {
            continue;
          }
        }
      } else if (
           MPISymbol::MPIOtherFence == transition.symbol
        || MPISymbol::MPIConditionAssertion == transition.symbol
      ) {
        continue;
      }

      outStream << Window->getId() << WindowSheet::colSep
        << windowState << WindowSheet::colSep
        << rankPredicate << WindowSheet::colSep
        << rankValue << WindowSheet::colSep
        << dumpInstruction(instruction) << WindowSheet::colSep
        << "\"" << psr::llvmIRToString(instruction) << "\""
        << std::endl;
    }
  }
}

