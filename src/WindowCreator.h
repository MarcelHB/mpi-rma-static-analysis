#ifndef H_WINDOW_TRACER
#define H_WINDOW_TRACER

#include <set>

#include "llvm/IR/CallSite.h"

#include "phasar/PhasarLLVM/IfdsIde/FlowFunction.h"

#include "Fact.h"
#include "MPIRMAUseProblem.h"
#include "MPISymbols.h"

class WindowCreator : public psr::FlowFunction<Fact*> {
  public:
    WindowCreator (
        llvm::ImmutableCallSite
      , MPIRMAUseProblem&
      , Fact*
      , MPISymbol
    );

    std::set<Fact*> computeTargets (Fact*);

  private:
    Fact *zeroValue;
    llvm::ImmutableCallSite callSite;
    MPIRMAUseProblem& problem;
    MPISymbol symbol;

    void assignOrderToFact (Fact*) const;
    void moveGhostsToFact (Fact*) const;
    void setWindowData (Fact*) const;

    unsigned getCommArgPosition () const;
    unsigned getInfoArgPosition () const;
    unsigned getMemArgPosition () const;
    unsigned getWindowArgPosition () const;
};

#endif
