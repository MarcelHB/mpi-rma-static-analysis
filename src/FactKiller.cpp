#include "FactKiller.h"
#include "MPISymbols.h"

#ifdef WITH_SIGNAL
#include <signal.h>
#endif

FactKiller::FactKiller (
    Fact::Type T
  , const llvm::Value *Value
  , const llvm::Instruction *Instruction
  , MPIRMAUseProblem &Problem
) : value(Value)
  , instruction(Instruction)
  , type(T)
  , problem(Problem)
{}

std::set<Fact*> FactKiller::computeTargets (Fact *factUnit) {
  if (factUnit->isType(type) && factUnit->knowsValue(value)) {
#ifdef WITH_SIGNAL
    if (!factUnit->isMarkedForPurging() && factUnit->isType(Fact::Type::Win)) {
      raise(SIGUSR2);
    }
#endif
    factUnit->markForPurging();

    if (factUnit->isType(Fact::Type::Win)) {
      /* Combine free-transition and summary. */
      auto SM = factUnit->getPath();
      SM->transit(WindowState::State::Freed, instruction, MPISymbol::MPIWinFree);

      Fact *ghostFact = problem.makeInitialFact(Fact::Type::WinGhost, factUnit->getZeroUnit());
      ghostFact->setGenerationData(instruction, nullptr, factUnit);

      return {ghostFact};
    }

    return {};

  // Kill ghost windows that are known to be tied to this communicator (possibly incomplete,
  // since the comm handle may have changed and yet be unknown), but this is just cleanup and
  // it may help keeping the calculation lower.
  } else if (
        factUnit->isType(Fact::Type::WinGhost)
    &&  type == Fact::Type::Comm
  ) {
    if (factUnit->getGeneratorUnit()->knowsCommunicator(value)) {
      factUnit->markForPurging();

      return {};
    }
  }

  return {factUnit};
}

