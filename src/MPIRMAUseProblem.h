#ifndef H_MPI_RMA_USE_PROBLEM
#define H_MPI_RMA_USE_PROBLEM

#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include <phasar/PhasarLLVM/ControlFlow/LLVMBasedICFG.h>
#include <phasar/PhasarLLVM/IfdsIde/DefaultIFDSTabulationProblem.h>
#include <phasar/Utils/LLVMShorthands.h>

#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Value.h"

#include "Fact.h"
#include "Settings.h"
#include "UnlockMap.h"

using MPIRMAIFDSClass = psr::DefaultIFDSTabulationProblem
    < const llvm::Instruction*
    , Fact*
    , const llvm::Function*
    , psr::LLVMBasedICFG&
    >;

class MPIRMAUseProblem : public MPIRMAIFDSClass {
  public:
    using D_Type = Fact;
    using FactsCollection = std::list<std::unique_ptr<D_Type>>;

    MPIRMAUseProblem (
        psr::LLVMBasedICFG&
      , const UnlockMap&
      , std::vector<std::string>&
      , const Settings&
    );

    D_Type* createZeroValue () override;

    std::shared_ptr<psr::FlowFunction<D_Type*>>
    getNormalFlowFunction (
        const llvm::Instruction*
      , const llvm::Instruction*
    ) override;

    std::shared_ptr<psr::FlowFunction<D_Type*>>
    getCallFlowFunction (
        const llvm::Instruction*
      , const llvm::Function*
    ) override;

    std::shared_ptr<psr::FlowFunction<D_Type*>>
    getRetFlowFunction (
        const llvm::Instruction*
      , const llvm::Function*
      , const llvm::Instruction*
      , const llvm::Instruction*
    ) override;

    std::shared_ptr<psr::FlowFunction<D_Type*>>
    getCallToRetFlowFunction (
        const llvm::Instruction*
      , const llvm::Instruction*
      , std::set<const llvm::Function *>
    ) override;

    std::shared_ptr<psr::FlowFunction<D_Type*>>
    getSummaryFlowFunction (
        const llvm::Instruction*
      , const llvm::Function*
    ) override;

    std::map<const llvm::Instruction *, std::set<D_Type*>>
    initialSeeds () override;

    bool isZeroValue(D_Type *d) const override {
      return d == _zeroValue.get();
    }

    std::string DtoString(D_Type *d) const override {
      return d->toString();
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

    void printDataFlowFact (std::ostream &os, D_Type *d) const override {
      os << d->toString();
    }

    void printMethod (std::ostream &os, const llvm::Function *m) const override {
      os << m->getName().str();
    }

    template<typename ... Args>
    D_Type* makeInitialFact (Args&& ... args) {
      facts.emplace_back(std::make_unique<D_Type>(std::forward<Args>(args) ...));

      return facts.back().get();
    }

    const FactsCollection& getFacts () const;

  private:
    std::vector<std::string> entryPoints;
    std::unique_ptr<D_Type> _zeroValue;
    FactsCollection facts;
    Settings::IntegerMode integerMode;

    static bool isKnownOffset (
        const llvm::GetElementPtrInst*
      , const std::set<const llvm::GetElementPtrInst*>*
    );
};

#endif
