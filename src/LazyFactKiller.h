#ifndef H_LAZY_FACT_KILLER
#define H_LAZY_FACT_KILLER

#include <set>

#include "llvm/IR/Value.h"

#include "phasar/PhasarLLVM/IfdsIde/FlowFunction.h"

#include "LazyFact.h"
#include "LazyMPIRMAUseProblem.h"

struct LazyFactKiller : public psr::FlowFunction<LazyFact*> {
  LazyFactKiller (
      LazyFact::Type
    , const llvm::Value*
    , const llvm::Instruction*
    , LazyMPIRMAUseProblem&
  );

  std::set<LazyFact*> computeTargets (LazyFact*);

  const llvm::Value *value;
  const llvm::Instruction *instruction;
  LazyFact::Type type;
  LazyMPIRMAUseProblem &problem;
};

#endif
