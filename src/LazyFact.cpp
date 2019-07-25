#include <sstream>

#include "LazyFact.h"

uint64_t LazyFact::instances = 0;

LazyFact::LazyFact (
    Type T
  , const UnlockMap &UnlockMap
  , uint32_t i
) : zeroUnit(this)
  , generatorUnit(nullptr)
  , previousUnit(nullptr)
  , previousFlowUnit(nullptr)
  , creatingInstruction(nullptr)
  , lockUntil(nullptr)
  , unlockMap(&UnlockMap)
  , creatorSymbol(MPISymbol::Null)
  , currentSymbol(MPISymbol::Null)
  , ID(instances++)
  , type(T)
  , markedForPurging(false)
  , isViolation(false)
  , hasBeenForked(false)
  , preventLock(false)
  , ithWindow(i)
  , currentWindow(0)
{}

LazyFact::LazyFact (
    Type T
  , LazyFact *ZeroUnit
) : zeroUnit(ZeroUnit)
  , generatorUnit(nullptr)
  , previousUnit(nullptr)
  , previousFlowUnit(nullptr)
  , creatingInstruction(nullptr)
  , lockUntil(nullptr)
  , unlockMap(nullptr)
  , creatorSymbol(MPISymbol::Null)
  , currentSymbol(MPISymbol::Null)
  , ID(instances++)
  , type(T)
  , markedForPurging(false)
  , isViolation(false)
  , hasBeenForked(false)
  , preventLock(false)
  , ithWindow(0)
  , currentWindow(0)
{
  if (LazyFact::Type::Win == T) {
    state.transit(WindowState::State::Created);
  }
}

LazyFact::LazyFact (
    const LazyFact &Other
  , const llvm::Instruction *Instruction
  , ForkActionType actionType
  , MPISymbol symbol
) : zeroUnit(Other.zeroUnit)
  , generatorUnit(Other.generatorUnit)
  , previousUnit(nullptr)
  , previousFlowUnit(&Other)
  , creatingInstruction(Other.creatingInstruction)
  , lockUntil(nullptr)
  , unlockMap(nullptr)
  , creatorSymbol(Other.creatorSymbol)
  , currentSymbol(symbol)
  , state(Other.state)
  , ID(instances++)
  , type(Other.type)
  , markedForPurging(false)
  , isViolation(Other.isViolation)
  , hasBeenForked(false)
  , preventLock(false)
  , ithWindow(0)
  , currentWindow(0)
  , loopEntries(Other.loopEntries)
{
  if (LazyFact::Type::Win == Other.type) {
    if (LazyFact::ForkActionType::PrepareLoop == actionType) {
      loopEntries.push(llvm::dyn_cast<llvm::BranchInst>(Instruction));
      preventLock = true;
      previousUnit = Other.previousUnit;
    } else if (LazyFact::ForkActionType::PrepareDoLoop == actionType) {
      loopEntries.push(llvm::dyn_cast<llvm::BranchInst>(Instruction));
      previousUnit = Other.previousUnit;
    } else if (LazyFact::ForkActionType::Replace == actionType) {
      creatingInstruction = Instruction;
      previousUnit = &Other;
    } else if (LazyFact::ForkActionType::ExitLoop == actionType) {
      loopEntries.pop();
      previousUnit = Other.previousUnit;
    }
  }
}

void LazyFact::addKnownValue (const llvm::Value *Value, bool initial) {
  if (isLocked()) {
    return;
  }

  insertValue(Value);

  if (auto bitcast = llvm::dyn_cast<llvm::BitCastInst>(Value)) {
    insertValue(bitcast->getOperand(0));
  }

  if (!initial) {
    return;
  }

  if (auto loadInstruction = llvm::dyn_cast<llvm::LoadInst>(Value)) {
    insertValue(loadInstruction->getPointerOperand());
  } else if (auto GEPInstruction = llvm::dyn_cast<llvm::GetElementPtrInst>(Value)) {
    if (auto loadInstruction = llvm::dyn_cast<llvm::LoadInst>(GEPInstruction->getPointerOperand())) {
      auto pointer = loadInstruction->getPointerOperand();
      insertValue(pointer);

      if (GEPInstruction->hasAllConstantIndices()) {
        zeroUnit->offsetAssociations[pointer].insert(GEPInstruction);
      }
    } else {
      if (GEPInstruction->hasAllConstantIndices()) {
        zeroUnit->offsetAssociations[GEPInstruction->getPointerOperand()].insert(GEPInstruction);
      }
    }

    insertValue(GEPInstruction->getPointerOperand());
  }
}

std::list<const llvm::Value*> LazyFact::addSomeKnownAddress (
    const llvm::Value *Value
  , LazyFact::ValueSet &Values
  , LazyFact::StackedValueSet &StackedValues
  , bool initial
) {
  if (isLocked()) {
    return {};
  }

  std::list<const llvm::Value*> insertedValues{};

  insertValueToSet(Value, Values, StackedValues);
  insertedValues.emplace_back(Value);

  if (!initial) {
    return insertedValues;
  }

  if (auto GEPInstruction = llvm::dyn_cast<llvm::GetElementPtrInst>(Value)) {
    if (auto loadInstruction = llvm::dyn_cast<llvm::LoadInst>(GEPInstruction->getPointerOperand())) {
      auto pointer = loadInstruction->getPointerOperand();
      insertValueToSet(pointer, Values, StackedValues);
      insertedValues.emplace_back(pointer);
    }

    insertValueToSet(GEPInstruction->getPointerOperand(), Values, StackedValues);
    insertedValues.emplace_back(GEPInstruction->getPointerOperand());
  } else if (auto castInstruction = llvm::dyn_cast<llvm::CastInst>(Value)) {
    auto castSource = castInstruction->getOperand(0);

    // Duplicate of above
    if (auto GEPInstruction = llvm::dyn_cast<llvm::GetElementPtrInst>(castSource)) {
      if (auto loadInstruction = llvm::dyn_cast<llvm::LoadInst>(GEPInstruction->getPointerOperand())) {
        auto pointer = loadInstruction->getPointerOperand();
        insertValueToSet(pointer, Values, StackedValues);
        insertedValues.emplace_back(pointer);
      }

      insertValueToSet(GEPInstruction->getPointerOperand(), Values, StackedValues);
      insertedValues.emplace_back(GEPInstruction->getPointerOperand());
    }

    insertValueToSet(castSource, Values, StackedValues);
    insertedValues.emplace_back(castSource);
  }

  return insertedValues;
}

bool LazyFact::awaitsLockAfter (const llvm::Instruction *Inst) const {
  return loopEntries.size() > 0 && loopEntries.top() == Inst;
}

bool LazyFact::canCreateWindowFact () const {
  return 0 == ithWindow || currentWindow + 1 == ithWindow;
}

const std::set<const llvm::GetElementPtrInst*>*
LazyFact::findOffsetInfo (const llvm::Value *Value) {
  auto searchResult = zeroUnit->offsetAssociations.find(Value);

  if (zeroUnit->offsetAssociations.end() != searchResult) {
    return &(searchResult->second);
  } else {
    return nullptr;
  }
}

const llvm::Instruction* LazyFact::getCreatingInstruction () const {
  return creatingInstruction;
}

LazyFact::ForkAction LazyFact::getForkAction (const llvm::Instruction *Instruction) {
  if (isLocked() && Instruction == lockUntil) {
    return ForkAction{ForkActionType::ExitLoop};
  } else if (isLocked()) {
    return ForkAction{ForkActionType::Keep};
  }

  if (loopEntries.size() > 0) {
    auto innermostLoopInst = loopEntries.top();
    auto branchSearchResult = zeroUnit->unlockMap->find(innermostLoopInst);

    if (zeroUnit->unlockMap->end() != branchSearchResult) {
      if (branchSearchResult->second.destination == Instruction) {
        if (preventLock) {
          preventLock = false;
        } else {
          return ForkAction{ForkActionType::Lock, branchSearchResult->second.exit};
        }
      } else if (branchSearchResult->second.exit == Instruction) {
        return ForkAction{ForkActionType::ExitLoop};
      }
    }
  }

  return ForkAction{ForkActionType::Keep};
}

uint64_t LazyFact::getId () const {
  return ID;
}

const LazyFact* LazyFact::getPreviousFact () const {
  return previousUnit;
}

WindowState& LazyFact::getState () {
  return state;
}

const WindowState& LazyFact::getState () const {
  return state;
}

LazyFact::Type LazyFact::getType () const {
  return type;
}

void LazyFact::increaseWindowCounter () {
  currentWindow += 1;
}

void LazyFact::insertValueToSet (
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

void LazyFact::insertValue (const llvm::Value *value) {
  insertValueToSet(
      value
    , primaryValues
    , stackedValues
  );
}

bool LazyFact::knowsValue (const llvm::Value *Value) const {
  bool knowsValue = knowsValueInSet(
      Value
    , primaryValues
    , stackedValues
    , {LazyFact::Type::Comm, LazyFact::Type::Win}
  );

  if (!knowsValue && nullptr != previousFlowUnit) {
    return previousFlowUnit->knowsValue(Value);
  } else {
    return knowsValue;
  }
}

bool LazyFact::knowsValueInSet (
    const llvm::Value *value
  , const LazyFact::ValueSet &primarySet
  , const LazyFact::StackedValueSet &stackedSet
  , const LazyFact::FactTypeSet &allowedTypes
) const {
  if (allowedTypes.find(type) == allowedTypes.end()) {
    return false;
  }

  bool onStackTop = false;

  if (stackedSet.size() > 0) {
    auto& topStackSet = stackedSet.top();
    onStackTop = topStackSet.find(value) != topStackSet.end();
  }

  return onStackTop || (primarySet.find(value) != primarySet.end());
}

bool LazyFact::isDroppedForForking () const {
  return hasBeenForked;
}

bool LazyFact::isLocked () const {
  return nullptr != lockUntil;
}

bool LazyFact::isLookingErrorneous () const {
  return isMarkedForViolation() || !state.hasFinalState();
}

bool LazyFact::isMarkedForPurging () const {
  return markedForPurging;
}

bool LazyFact::isMarkedForViolation () const {
  return isViolation;
}

bool LazyFact::isType (const Type T) const {
  return T == type;
}

void LazyFact::lock (const llvm::Instruction *UntilInstruction) {
  lockUntil = UntilInstruction;
}

void LazyFact::markForPurging (const llvm::Instruction *Instruction) {
  markedForPurging = true;

  if (nullptr != Instruction) {
    hasBeenForked = true;
  }
}

void LazyFact::markForViolation () {
  isViolation = true;
}

void LazyFact::popTokenLayers () {
  if (!isLocked()) {
    if (stackedValues.size() > 0) {
      stackedValues.pop();
    }
  }
}

void LazyFact::pushTokenLayers () {
  if (!isLocked()) {
    stackedValues.push({});
  }
}

void LazyFact::setGenerationData (
    const llvm::Instruction *CreatingInstruction
  , LazyFact *GeneratorUnit
) {
  creatingInstruction = CreatingInstruction;

  if (isType(LazyFact::Type::Win)) {
    generatorUnit = GeneratorUnit;
  }
}

void LazyFact::setWindowInformation (MPISymbol Symbol) {
  creatorSymbol = Symbol;
}

std::string LazyFact::toString () const {
  if (LazyFact::Type::Null == type) {
    return "null";
  } else {
    std::stringstream ss;

    switch (type) {
      case LazyFact::Type::Comm:
        ss << "Comm";
        break;
      case LazyFact::Type::Win:
        ss << "Win";

        if (auto callInst = llvm::dyn_cast<llvm::CallInst>(creatingInstruction)) {
          auto callTargetFunction = callInst->getCalledFunction();

          if (callTargetFunction != nullptr && callTargetFunction->isDeclaration()) {
            ss << " : " << callTargetFunction->getName().str();
          }
        }

        break;
      default:
        break;
    }

    ss << " ("  << ID << "):";

    return ss.str();
  }
}

void LazyFact::unlock () {
  if (isLocked()) {
    lockUntil = nullptr;
  }
}
