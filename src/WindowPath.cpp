#include "WindowPath.h"

WindowPath::WindowPath () :
    previousPath(nullptr)
  , violatingInstruction(nullptr)
  , transitions({})
  , rankPredicate(llvm::CmpInst::Predicate::BAD_ICMP_PREDICATE)
{}

WindowPath::WindowPath (
    const WindowPath &Other
) : previousPath(&Other)
  , violatingInstruction(Other.violatingInstruction)
  , state(Other.state)
  , transitions({})
  , rankPredicate(llvm::CmpInst::Predicate::BAD_ICMP_PREDICATE)
{}

const WindowPath* WindowPath::getPreviousPath () const {
  return previousPath;
}

llvm::CmpInst::Predicate WindowPath::getRankPredicate () const {
  return rankPredicate;
}

int64_t WindowPath::getRankValue () const {
  return rankConstantValue;
}

const llvm::Instruction* WindowPath::getViolatingInstruction () const {
  return violatingInstruction;
}

const WindowPath::Transitions& WindowPath::getTransitions () const {
  return transitions;
}

bool WindowPath::hasRankPredicate () const {
  return rankPredicate != llvm::CmpInst::BAD_ICMP_PREDICATE;
}

bool WindowPath::hasFinalState () const {
  return state.hasFinalState();
}

void WindowPath::setRankAttributes (
    llvm::CmpInst::Predicate Predicate
  , int64_t RankValue
) {
  rankPredicate = Predicate;
  rankConstantValue = RankValue;
}

void WindowPath::setViolatingInstruction (const llvm::Instruction *Instruction) {
  violatingInstruction = Instruction;
}

void WindowPath::transit (
    WindowState::State NextState
  , const llvm::Instruction *Instruction
  , MPISymbol Symbol
  , const llvm::CallInst *Generator
) {
  if (nullptr != getViolatingInstruction()) {
    return;
  }

  transitions.emplace_back(NextState, Instruction, Symbol, Generator);

  if (!state.transit(NextState)) {
    setViolatingInstruction(Instruction);
  }
}

