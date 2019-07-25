#ifndef H_WINDOW_TRACER
#define H_WINDOW_TRACER

#include <set>

#include "llvm/IR/CallSite.h"

#include "phasar/PhasarLLVM/IfdsIde/FlowFunction.h"

#include "LazyFact.h"
#include "LazyMPIRMAUseProblem.h"
#include "MPISymbols.h"

class LazyWindowCreator : public psr::FlowFunction<LazyFact*> {
  public:
    LazyWindowCreator (
        llvm::ImmutableCallSite
      , LazyMPIRMAUseProblem&
      , LazyFact*
      , MPISymbol
    );

    std::set<LazyFact*> computeTargets (LazyFact*);

  private:
    LazyFact *zeroValue;
    llvm::ImmutableCallSite callSite;
    LazyMPIRMAUseProblem& problem;
    MPISymbol symbol;

    unsigned getCommArgPosition () const;
    unsigned getWindowArgPosition () const;
};

#endif
