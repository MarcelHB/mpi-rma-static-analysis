#ifdef WITH_SIGNAL
#include <signal.h>
#endif

#include "llvm/IR/Constants.h"
#include "llvm/IR/GlobalVariable.h"

#include "LazyWindowCreator.h"

LazyWindowCreator::LazyWindowCreator (
    llvm::ImmutableCallSite CallSite
  , LazyMPIRMAUseProblem &Problem
  , LazyFact *ZV
  , MPISymbol Symbol
) : zeroValue(ZV)
  , callSite(CallSite)
  , problem(Problem)
  , symbol(Symbol)
{}

std::set<LazyFact*> LazyWindowCreator::computeTargets (LazyFact* value) {
  if (value->isType(LazyFact::Type::Comm)) {
    if (value->knowsValue(callSite.getArgument(getCommArgPosition()))) {
      if (zeroValue->canCreateWindowFact()) {
        LazyFact *generatedLazyFact =
          problem.makeInitialFact(
              LazyFact::Type::Win
            , zeroValue
          );

        generatedLazyFact->addKnownValue(callSite.getArgument(getWindowArgPosition()), true);
        generatedLazyFact->setWindowInformation(symbol);
        generatedLazyFact->setGenerationData(
            callSite.getInstruction()
          , value
        );

#ifdef WITH_SIGNAL
        raise(SIGUSR1);
#endif
        zeroValue->increaseWindowCounter();

        return {value, generatedLazyFact};
      } else {
        zeroValue->increaseWindowCounter();
      }
    }
  } else if (value->isType(LazyFact::Type::Null)) {
    if (zeroValue->canCreateWindowFact()) {
      auto constant = llvm::dyn_cast<llvm::ConstantInt>(callSite.getArgument(getCommArgPosition()));

      // Fortran module variables/constants.
      if (!constant) {
        if (auto bitcast = llvm::dyn_cast<llvm::BitCastInst>(callSite.getArgument(getCommArgPosition()))) {
          auto target = bitcast->getOperand(0);
          auto global = llvm::dyn_cast<llvm::GlobalVariable>(target);

          if (global && global->isConstant() && !global->getInitializer()->isNullValue()) {
            constant = llvm::dyn_cast<llvm::ConstantInt>(global->getInitializer());
          }
        }
      }

      if (constant) {
        LazyFact *generatedLazyFact =
          problem.makeInitialFact(
              LazyFact::Type::Win
            , zeroValue
          );

        generatedLazyFact->addKnownValue(callSite.getArgument(getWindowArgPosition()), true);
        generatedLazyFact->setWindowInformation(symbol);
        generatedLazyFact->setGenerationData(
            callSite.getInstruction()
          , nullptr
        );

#ifdef WITH_SIGNAL
        raise(SIGUSR1);
#endif
        zeroValue->increaseWindowCounter();

        return {value, generatedLazyFact};
      }
    } else {
      zeroValue->increaseWindowCounter();
    }
  }

  return {value};
}

unsigned LazyWindowCreator::getCommArgPosition () const {
  switch (symbol) {
    case MPISymbol::MPIWinAllocate:
    case MPISymbol::MPIWinAllocateShared: return 3;
    case MPISymbol::MPIWinCreate: return 4;
    case MPISymbol::MPIWinCreateDynamic: return 1;
    default: return -1;
  }
}

unsigned LazyWindowCreator::getWindowArgPosition () const {
  switch (symbol) {
    case MPISymbol::MPIWinAllocate:
    case MPISymbol::MPIWinAllocateShared:
    case MPISymbol::MPIWinCreate: return 5;
    case MPISymbol::MPIWinCreateDynamic: return 2;
    default: return -1;
  }
}

