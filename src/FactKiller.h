#ifndef H_FACT_KILLER
#define H_FACT_KILLER

#include <set>

#include "llvm/IR/Value.h"

#include "phasar/PhasarLLVM/IfdsIde/FlowFunction.h"

#include "Fact.h"
#include "MPIRMAUseProblem.h"

struct FactKiller : public psr::FlowFunction<Fact*> {
  FactKiller (
      Fact::Type
    , const llvm::Value*
    , const llvm::Instruction*
    , MPIRMAUseProblem&
  );

  std::set<Fact*> computeTargets (Fact*);

  const llvm::Value *value;
  const llvm::Instruction *instruction;
  Fact::Type type;
  MPIRMAUseProblem &problem;
};

#endif
