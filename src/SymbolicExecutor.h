#ifndef H_SYMBOLIC_EXECUTOR
#define H_SYMBOLIC_EXECUTOR

#include "llvm/IR/CallSite.h"

#include "phasar/PhasarLLVM/IfdsIde/FlowFunction.h"

#include "Fact.h"
#include "MPISymbols.h"
#include "WindowPath.h"

struct SymbolicExecutor : public psr::FlowFunction<Fact*> {

  SymbolicExecutor (const llvm::CallInst*, MPISymbol);

  std::set<Fact*> computeTargets (Fact*);
  void executeOnSM (WindowPath*);

  static unsigned getWindowPositionFromMPISymbol (const MPISymbol);
  static std::set<unsigned> getMemoryPositionsFromMPISymbol (const MPISymbol);

  const llvm::CallInst *callInstruction;
  unsigned windowPos;
  const llvm::Value *windowValue;
  MPISymbol symbol;
};

#endif
