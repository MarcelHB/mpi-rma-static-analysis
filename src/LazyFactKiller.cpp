#ifdef WITH_SIGNAL
#include <signal.h>
#endif

#include "LazyFactKiller.h"
#include "MPISymbols.h"
#include "WindowState.h"

LazyFactKiller::LazyFactKiller (
    LazyFact::Type T
  , const llvm::Value *Value
  , const llvm::Instruction *Instruction
  , LazyMPIRMAUseProblem &Problem
) : value(Value)
  , instruction(Instruction)
  , type(T)
  , problem(Problem)
{}

std::set<LazyFact*> LazyFactKiller::computeTargets (LazyFact *factUnit) {
  if (factUnit->isType(type) && factUnit->knowsValue(value)) {

    if (factUnit->isType(LazyFact::Type::Win)) {
      factUnit->markForPurging(instruction);

      LazyFact* newFact = problem.makeInitialFact(
          *factUnit
        , instruction
        , LazyFact::ForkActionType::Replace
        , MPISymbol::MPIWinFree
      );

#ifdef WITH_SIGNAL
      raise(SIGUSR2);
#endif

      auto &SM = newFact->getState();
      if (!SM.transit(WindowState::State::Freed)) {
        newFact->markForViolation();
      }
    } else {
      factUnit->markForPurging();
    }

    return {};
  }

  return {factUnit};
}
