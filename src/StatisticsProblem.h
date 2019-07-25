#ifndef H_STATISTICS_PROBLEM
#define H_STATISTICS_PROBLEM

#include <memory>
#include <vector>

#include <phasar/PhasarLLVM/ControlFlow/LLVMBasedICFG.h>
#include <phasar/PhasarLLVM/IfdsIde/DefaultIFDSTabulationProblem.h>
#include <phasar/Utils/LLVMShorthands.h>

#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Value.h"

#include "Settings.h"
#include "StatisticsFact.h"
#include "UnlockMap.h"

using SD_Type = StatisticsFact;
using StatisticsClass = psr::DefaultIFDSTabulationProblem
    < const llvm::Instruction*
    , SD_Type*
    , const llvm::Function*
    , psr::LLVMBasedICFG&
    >;

class StatisticsProblem : public StatisticsClass {
  public:
    StatisticsProblem (
        psr::LLVMBasedICFG&
      , const UnlockMap&
      , std::vector<std::string>&
      , Settings::IntegerMode
    );

    SD_Type* createZeroValue () override;

    std::shared_ptr<psr::FlowFunction<SD_Type*>>
    getNormalFlowFunction (
        const llvm::Instruction*
      , const llvm::Instruction*
    ) override;

    std::shared_ptr<psr::FlowFunction<SD_Type*>>
    getCallFlowFunction (
        const llvm::Instruction*
      , const llvm::Function*
    ) override;

    std::shared_ptr<psr::FlowFunction<SD_Type*>>
    getRetFlowFunction (
        const llvm::Instruction*
      , const llvm::Function*
      , const llvm::Instruction*
      , const llvm::Instruction*
    ) override;

    std::shared_ptr<psr::FlowFunction<SD_Type*>>
    getCallToRetFlowFunction (
        const llvm::Instruction*
      , const llvm::Instruction*
      , std::set<const llvm::Function *>
    ) override;

    std::shared_ptr<psr::FlowFunction<SD_Type*>>
    getSummaryFlowFunction (
        const llvm::Instruction*
      , const llvm::Function*
    ) override;

    std::map<const llvm::Instruction *, std::set<SD_Type*>>
    initialSeeds () override;

    bool isZeroValue(SD_Type *d) const override {
      return d == _zeroValue.get();
    }

    std::string DtoString(SD_Type *d) const override {
      return std::string{"n/a"};
    }

    std::string NtoString(const llvm::Instruction *n) const override {
      return psr::llvmIRToString(n);
    }

    std::string MtoString(const llvm::Function *m) const override {
      return psr::llvmIRToString(m);
    }

    void printNode (std::ostream &os, const llvm::Instruction *n) const override {
      os << psr::llvmIRToString(n);
    }

    void printDataFlowFact (std::ostream &os, SD_Type *d) const override {
    }

    void printMethod (std::ostream &os, const llvm::Function *m) const override {
      os << m->getName().str();
    }

    void finalize ();
    const StatisticsFact::Record& getRecord () const;

  private:
    std::vector<std::string> entryPoints;
    std::unique_ptr<SD_Type> _zeroValue;
    bool finalized;
};

#endif
