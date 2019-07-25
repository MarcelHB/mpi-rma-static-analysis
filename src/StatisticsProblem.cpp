#include "StatisticsProblem.h"

#include "phasar/PhasarLLVM/IfdsIde/FlowFunction.h"

StatisticsProblem::StatisticsProblem (
    psr::LLVMBasedICFG &ICFG
  , const ::UnlockMap &UnlockMap
  , std::vector<std::string> &EntryPoints
  , Settings::IntegerMode IntegerMode
) : StatisticsClass(ICFG)
  , entryPoints(EntryPoints)
  , _zeroValue(std::make_unique<SD_Type>(UnlockMap, IntegerMode))
  , finalized(false)
{
  DefaultIFDSTabulationProblem::zerovalue = createZeroValue();
}

struct StatisticsFlow : public psr::FlowFunction<SD_Type*> {
  const llvm::Instruction *instruction;
  const llvm::Function *function;
  SD_Type::Type type;
  std::string token;

  StatisticsFlow (
      const llvm::Instruction *Instruction
    , SD_Type::Type T
    , const llvm::Function *Function = nullptr
  ) : instruction(Instruction)
    , function(Function)
    , type(T)
  {}

  std::set<SD_Type*> computeTargets (SD_Type *Value) {
    Value->consume(instruction, type, function);

    if (StatisticsFact::Type::CRF == type) {
      Value->popLayer();
    }

    return {Value};
  }
};

SD_Type* StatisticsProblem::createZeroValue () {
  return _zeroValue.get();
}

std::shared_ptr<psr::FlowFunction<SD_Type*>>
StatisticsProblem::getNormalFlowFunction (
    const llvm::Instruction *CurrentInstruction
  , const llvm::Instruction *SuccessorInstruction
) {
  return std::make_shared<StatisticsFlow>(CurrentInstruction, StatisticsFact::Type::NF);
}

std::shared_ptr<psr::FlowFunction<SD_Type*>>
StatisticsProblem::getCallFlowFunction (
    const llvm::Instruction *callInstruction
  , const llvm::Function *targetFunction
) {
  return std::make_shared<StatisticsFlow>(callInstruction, StatisticsFact::Type::CF, targetFunction);
}

std::shared_ptr<psr::FlowFunction<SD_Type*>>
StatisticsProblem::getRetFlowFunction (
    const llvm::Instruction *callSite
  , const llvm::Function *calleeMethod
  , const llvm::Instruction *exitInstruction
  , const llvm::Instruction*
) {
  return std::make_shared<StatisticsFlow>(exitInstruction, StatisticsFact::Type::RF);
}

std::shared_ptr<psr::FlowFunction<SD_Type*>>
StatisticsProblem::getCallToRetFlowFunction (
    const llvm::Instruction *callSite
  , const llvm::Instruction *retSite
  , std::set<const llvm::Function *> callees
) {
  return std::make_shared<StatisticsFlow>(retSite, StatisticsFact::Type::CRF);
}

std::shared_ptr<psr::FlowFunction<SD_Type*>>
StatisticsProblem::getSummaryFlowFunction (
    const llvm::Instruction *callInstruction
  , const llvm::Function *targetFunction
) {
  if (targetFunction->isDeclaration()) {
    return std::make_shared<StatisticsFlow>(callInstruction, StatisticsFact::Type::SF, targetFunction);
  } else {
    return nullptr;
  }
}

std::map<const llvm::Instruction *, std::set<SD_Type*>>
StatisticsProblem::initialSeeds () {
  std::map<const llvm::Instruction *, std::set<SD_Type*>> seedMap;

  for (auto &entryPoint : entryPoints) {
    seedMap.insert(
      std::make_pair(
          &icfg.getMethod(entryPoint)->front().front()
        , std::set<SD_Type*>({zeroValue()})
      )
    );
  }

  return seedMap;
}

void StatisticsProblem::finalize () {
  if (!finalized) {
    finalized = true;
    _zeroValue->finalize();
  }
}

const StatisticsFact::Record& StatisticsProblem::getRecord () const {
  return _zeroValue->getRecord();
}
