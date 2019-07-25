#include "llvm/IR/Constants.h"
#include "llvm/IR/CallSite.h"

#include "phasar/Utils/LLVMShorthands.h"

#include "StatisticsFact.h"

StatisticsFact::StatisticsFact (
    const ::UnlockMap &UnlockMap
  , Settings::IntegerMode IntegerMode
)
  : wormholingUntil(nullptr)
  , unlockMap(UnlockMap)
  , silent(true)
  , integerMode(IntegerMode)
{}

void StatisticsFact::addKnownValue (const llvm::Value *Value, bool initial) {
  insertValue(Value);

  if (auto bitcast = llvm::dyn_cast<llvm::BitCastInst>(Value)) {
    insertValue(bitcast->getOperand(0));
  }

  if (!initial) {
    return;
  }

  if (auto GEPInstruction = llvm::dyn_cast<llvm::GetElementPtrInst>(Value)) {
    if (auto loadInstruction = llvm::dyn_cast<llvm::LoadInst>(GEPInstruction->getPointerOperand())) {
      auto pointer = loadInstruction->getPointerOperand();
      insertValue(pointer);

      if (GEPInstruction->hasAllConstantIndices()) {
        offsetAssociations[pointer].insert(GEPInstruction);
      }
    } else {
      if (GEPInstruction->hasAllConstantIndices()) {
        offsetAssociations[GEPInstruction->getPointerOperand()].insert(GEPInstruction);
      }
    }

    insertValue(GEPInstruction->getPointerOperand());
  }
}

void StatisticsFact::consume (
    const llvm::Instruction *Instruction
  , StatisticsFact::Type Type
  , const llvm::Function *Function
) {
  /**
   * skip:
   * * if is CRF (also appears in NF)
   * * if is CF on declaration (also appears in SF)
   */
  if ( StatisticsFact::Type::CRF == Type
    || (StatisticsFact::Type::CF == Type && nullptr != Function && Function->isDeclaration())) {
    return;
  }

  record.numInstructionsTotal += 1;
  if (!silent) {
    record.numInstructions += 1;
  }

  if (Settings::IntegerMode::None != integerMode) {
    if (auto bitCastInst = llvm::dyn_cast<llvm::BitCastInst>(Instruction)) {
      if (knowsValue(bitCastInst->getOperand(0))) {
        addKnownValue(bitCastInst);
      }
    } else if (auto loadInst = llvm::dyn_cast<llvm::LoadInst>(Instruction)) {
      if (knowsValue(loadInst->getPointerOperand())) {
        addKnownValue(loadInst);
      }
    } else if (auto storeInst = llvm::dyn_cast<llvm::StoreInst>(Instruction)) {
      if (knowsValue(storeInst->getValueOperand())) {
        addKnownValue(storeInst->getPointerOperand());
      }

      if (auto constant = llvm::dyn_cast<llvm::ConstantInt>(storeInst->getValueOperand())) {
        if (constant->getType()->isIntegerTy()) {
          if (!knowsValue(storeInst->getPointerOperand())) {
            record.numIntegers += 1;
            silent = false;

            addKnownValue(storeInst->getPointerOperand());
          }
        }
      }
    }
  }

  if (auto callInst = llvm::dyn_cast<llvm::CallInst>(Instruction)) {
    if ( nullptr != Function
      && Function->isDeclaration()
      && ( Function->getName().startswith_lower("MPI_Win_allocate")
        || Function->getName().startswith_lower("MPI_Win_create")
       )
    ) {
      silent = false;
      record.numWindows += 1;
    } else if (
         nullptr != Function
      && Function->isDeclaration()
      && (   Function->getName().startswith_lower("MPI_Win_")
          || Function->getName().startswith_lower("MPI_Accumulate")
          || Function->getName().startswith_lower("MPI_Compare_and_swap")
          || Function->getName().startswith_lower("MPI_Fetch_and_op")
          || Function->getName().startswith_lower("MPI_Get_accumulate")
          || Function->getName().equals("MPI_Get")
          || Function->getName().equals_lower("MPI_Get_")
          || Function->getName().startswith_lower("MPI_Put")
         )
    ) {
      record.numWinCalls += 1;
    } else if (
         Settings::IntegerMode::None != integerMode
      && nullptr != Function
      && !Function->isDeclaration()
    ) {
      // Since we do this only once per call site at all, we can also do that here.
      llvm::ImmutableCallSite callSite(callInst);
      pushLayer();

      for (unsigned idx = 0; idx < callSite.getNumArgOperands(); ++idx) {
        if (knowsValue(callSite.getArgOperand(idx))) {
          addKnownValue(psr::getNthFunctionArgument(Function, idx));
        }
      }
    }
  } else if (auto branchInst = llvm::dyn_cast<llvm::BranchInst>(Instruction)) {
    if (branchInst->isConditional()) {
      if (!isForkingBack(branchInst)) {
        record.numBranches += 1;
      }
    }
  }
}

const StatisticsFact::Record& StatisticsFact::getRecord () const {
  return record;
}

bool StatisticsFact::isForkingBack (const llvm::BranchInst *branchInst) const {
  if (!branchInst->isConditional()) {
    return false;
  }

  auto branchSearchResult = unlockMap.find(branchInst);
  if (unlockMap.end() != branchSearchResult) {
    for (int i = 0; i < 2; ++i) {
      auto destBB = branchInst->getSuccessor(i);
      auto firstBBInst = &(destBB->front());

      if (branchSearchResult->second.destination == firstBBInst) {
        return true;
      }
    }
  }

  return false;
}

void StatisticsFact::insertValue (const llvm::Value *value) {
  insertValueToSet(
      value
    , primaryValues
    , stackedValues
  );
}

void StatisticsFact::insertValueToSet (
    const llvm::Value *value
  , ValueSet &primarySet
  , StackedValueSet &stackedSet
) {
  auto globalVariable = llvm::dyn_cast<llvm::GlobalVariable>(value);

  if (globalVariable || 0 == stackedSet.size()) {
    primarySet.insert(value);
  } else {
    stackedSet.top().insert(value);
  }
}

bool StatisticsFact::knowsValue (const llvm::Value *Value) const {
  return knowsValueInSet(
      Value
    , primaryValues
    , stackedValues
  );
}

bool StatisticsFact::knowsValueInSet (
    const llvm::Value *value
  , const StatisticsFact::ValueSet &primarySet
  , const StatisticsFact::StackedValueSet &stackedSet
) const {
  bool onStackTop = false;

  if (stackedSet.size() > 0) {
    auto& topStackSet = stackedSet.top();
    onStackTop = topStackSet.find(value) != topStackSet.end();
  }

  return onStackTop || (primarySet.find(value) != primarySet.end());
}

/**
 * Finally correct the number of instructions: conditional `br` appear
 * twice in NF, once per edge.
 */
void StatisticsFact::finalize () {
  record.numBranches /= 2;
  record.numInstructions -= record.numBranches;
}

void StatisticsFact::popLayer () {
  if (stackedValues.size() > 0) {
    stackedValues.pop();
  }
}

void StatisticsFact::pushLayer () {
  stackedValues.push({});
}

