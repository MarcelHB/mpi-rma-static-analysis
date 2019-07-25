#ifndef H_LAZY_SYMBOLIC_EXECUTOR
#define H_LAZY_SYMBOLIC_EXECUTOR

#include "llvm/IR/CallSite.h"

#include "phasar/PhasarLLVM/IfdsIde/FlowFunction.h"

#include "LazyFact.h"
#include "LazyMPIRMAUseProblem.h"
#include "MPISymbols.h"

struct LazySymbolicExecutor : public psr::FlowFunction<LazyFact*> {

  LazySymbolicExecutor (
      const llvm::CallInst*
    , MPISymbol
    , LazyMPIRMAUseProblem&
  );

  std::set<LazyFact*> computeTargets (LazyFact*);

  static unsigned getWindowPositionFromMPISymbol (const MPISymbol);

  const llvm::CallInst *callInstruction;
  unsigned windowPos;
  const llvm::Value *windowValue;
  MPISymbol symbol;
  LazyMPIRMAUseProblem &problemInstance;
};

#endif
