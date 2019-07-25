#include <sstream>
#include <utility>

#include "llvm/IR/Constants.h"
#include "llvm/IR/GlobalVariable.h"

#include <phasar/Utils/LLVMShorthands.h>

#include "Fact.h"
#include "MPISymbols.h"

uint64_t Fact::instances = 0;

Fact::Fact (
    Type T
  , const UnlockMap &UnlockMap
  , uint32_t i
) :
    firstUnit(this)
  , zeroUnit(this)
  , communicator(nullptr)
  , generatorUnit(nullptr)
  , previousUnit(nullptr)
  , creatingInstruction(nullptr)
  , localPath(nullptr)
  , branchMarker(nullptr)
  , lockUntil(nullptr)
  , unlockMap(&UnlockMap)
  , creatorSymbol(MPISymbol::Null)
  , ID(instances++)
  , type(T)
  , dontFork(false)
  , markedForPurging(false)
  , hasBeenForked(false)
  , nthUnit(0)
  , constantValue(0)
  , constantValueLost(true)
  , ithWindow(i)
  , currentWindow(0)
{
  if (Fact::Type::Win == T) {
    localPath = registerStateMachine();
  }
}

Fact::Fact (
    Type T
  , Fact *ZeroUnit
  , int64_t ConstantValue
  , bool DontFork
) :
    firstUnit(this)
  , zeroUnit(ZeroUnit)
  , communicator(nullptr)
  , generatorUnit(nullptr)
  , previousUnit(nullptr)
  , creatingInstruction(nullptr)
  , localPath(nullptr)
  , branchMarker(nullptr)
  , lockUntil(nullptr)
  , unlockMap(nullptr)
  , creatorSymbol(MPISymbol::Null)
  , ID(instances++)
  , type(T)
  , dontFork(DontFork)
  , markedForPurging(false)
  , hasBeenForked(false)
  , nthUnit(0)
  , constantValue(ConstantValue)
  , constantValueLost(false)
  , ithWindow(0)
  , currentWindow(0)
{
  if (Fact::Type::Win == T) {
    localPath = registerStateMachine();
  }
}

Fact::Fact (
    const Fact &Other
  , const llvm::Instruction *Instruction
) : firstUnit(Other.firstUnit)
  , zeroUnit(Other.zeroUnit)
  , communicator(Other.communicator)
  , generatorUnit(Other.generatorUnit)
  , previousUnit(&Other)
  , creatingInstruction(Other.creatingInstruction)
  , branchMarker(nullptr)
  , lockUntil(nullptr)
  , unlockMap(nullptr)
  , creatorSymbol(Other.creatorSymbol)
  , ID(instances++)
  , type(Other.type)
  , dontFork(Other.dontFork)
  , markedForPurging(false)
  , hasBeenForked(false)
  , nthUnit(0) // irrelevant
  , affectedLocalMemoryValues(Other.affectedLocalMemoryValues)
  , localMemoryGenerators(Other.localMemoryGenerators)
  , ancestors(Other.ancestors)
  , constantValue(Other.constantValue)
  , constantValueLost(Other.constantValueLost)
  , ithWindow(0)
  , currentWindow(0)
{
  if (isType(Fact::Type::Win)) {
    localPath = registerStateMachine(Other.localPath);
    registerWindowPassage(creatingInstruction, this);
  }
}

void Fact::addAncestry (const Fact *Fact) {
  ancestors.insert(Fact);
}

/**
 * For addresses of window memory, we currently work in a more relaxed node:
 * Instead of assuming constant offsets (such as handles in structs), we
 * just look for the base address to match.
 */
void Fact::addKnownAddress (const llvm::Value *Value, bool initial) {
  addSomeKnownAddress(
      Value
    , primaryMemoryAddresses
    , stackedMemoryAddresses
    , initial
  );
}

void Fact::addKnownInitialLocalAddress (
    const llvm::Value *Value
  , const llvm::Instruction *Instruction
) {
  auto values = addSomeKnownAddress(
      Value
    , primaryLocalMemoryAddresses
    , stackedLocalMemoryAddresses
    , true
  );

  auto &instructionSet = affectedLocalMemoryValues[Instruction];

  for (auto value : values) {
    instructionSet.insert(value);
    localMemoryGenerators[value] = Instruction;
  }
}

void Fact::addKnownLocalAddress (
    const llvm::Value *Value
  , const llvm::Value *LastValue
) {
  addSomeKnownAddress(
      Value
    , primaryLocalMemoryAddresses
    , stackedLocalMemoryAddresses
    , false
  );

  auto searchResult = localMemoryGenerators.find(LastValue);

  if (localMemoryGenerators.end() != searchResult) {
    auto instruction = searchResult->second;
    localMemoryGenerators[Value] = instruction;
    affectedLocalMemoryValues[instruction].insert(Value);
  }
}

// Duplicate of `addKnownInitialLocalAddress`
void Fact::addKnownInitialSharedAddress (
    const llvm::Value *Value
  , const llvm::Instruction *Instruction
) {
  auto values = addSomeKnownAddress(
      Value
    , primarySharedMemoryAddresses
    , stackedSharedMemoryAddresses
    , true
  );

  auto &instructionSet = affectedLocalMemoryValues[Instruction];

  for (auto value : values) {
    instructionSet.insert(value);
    localMemoryGenerators[value] = Instruction;
  }
}

// Duplicate of `addKnownLocalAddress`
void Fact::addKnownSharedAddress (
    const llvm::Value *Value
  , const llvm::Value *LastValue
) {
  addSomeKnownAddress(
      Value
    , primarySharedMemoryAddresses
    , stackedSharedMemoryAddresses
    , false
  );

  auto searchResult = localMemoryGenerators.find(LastValue);

  if (localMemoryGenerators.end() != searchResult) {
    auto instruction = searchResult->second;
    localMemoryGenerators[Value] = instruction;
    affectedLocalMemoryValues[instruction].insert(Value);
  }
}

/**
 * Duplicate of `addKnownValue`.
 */
void Fact::addKnownRankValue (const llvm::Value *Value, bool initial) {
  if (isLocked()) {
    return;
  }

  insertRankValue(Value);

  if (auto bitcast = llvm::dyn_cast<llvm::BitCastInst>(Value)) {
    insertRankValue(bitcast->getOperand(0));
  }

  if (!initial) {
    return;
  }

  if (auto GEPInstruction = llvm::dyn_cast<llvm::GetElementPtrInst>(Value)) {
    if (auto loadInstruction = llvm::dyn_cast<llvm::LoadInst>(GEPInstruction->getPointerOperand())) {
      auto pointer = loadInstruction->getPointerOperand();
      insertRankValue(pointer);

      if (GEPInstruction->hasAllConstantIndices()) {
        zeroUnit->offsetAssociations[pointer].insert(GEPInstruction);
      }
    } else {
      if (GEPInstruction->hasAllConstantIndices()) {
        zeroUnit->offsetAssociations[GEPInstruction->getPointerOperand()].insert(GEPInstruction);
      }
    }

    insertRankValue(GEPInstruction->getPointerOperand());
  }
}

/**
 * Known values work for direct loads or loads after GEP, but then only for offsets
 * that are constants, which should be a viable option for most real-world cases,
 * i. e. classes/structs. Nonetheless, no arrays with dyn. offsets right now.
 */
void Fact::addKnownValue (const llvm::Value *Value, bool initial) {
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

std::list<const llvm::Value*> Fact::addSomeKnownAddress (
    const llvm::Value *Value
  , Fact::ValueSet &Values
  , Fact::StackedValueSet &StackedValues
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

bool Fact::canCreateWindowFact () const {
  return 0 == ithWindow || currentWindow + 1 == ithWindow;
}

const std::set<const llvm::GetElementPtrInst*>*
Fact::findOffsetInfo (const llvm::Value *Value) {
  auto searchResult = zeroUnit->offsetAssociations.find(Value);

  if (zeroUnit->offsetAssociations.end() != searchResult) {
    return &(searchResult->second);
  } else {
    return nullptr;
  }
}

const llvm::BranchInst* Fact::getBranchMarker () const {
  return branchMarker;
}

WindowPath* Fact::getPath () const {
  return localPath;
}

const Fact::CallValueMap& Fact::getCallValueMap () const {
  return affectedLocalMemoryValues;
}

const Fact::ValueCallMap& Fact::getValueCallMap () const {
  return localMemoryGenerators;
}

MPISymbol Fact::getCreatorSymbol () const {
  return creatorSymbol;
}

int64_t Fact::getCurrentInteger () const {
  return constantValue;
}

const llvm::Value* Fact::getCommunicator () const {
  return communicator;
}

Fact::ForkAction Fact::getForkAction (const llvm::Instruction *Instruction) const {
  if (  isType(Fact::Type::Win)
    &&  nullptr != localPath->getViolatingInstruction()
  ) {
    return ForkAction{ForkActionType::Drop};
  }

  if (dontFork) {
    return ForkAction{ForkActionType::Keep};
  }

  if (isLocked() && Instruction == lockUntil) {
    return ForkAction{ForkActionType::Unlock};
  } else if (isLocked()) {
    return ForkAction{ForkActionType::Keep};
  }

  auto branchSearchResult = zeroUnit->unlockMap->find(branchMarker);
  if (zeroUnit->unlockMap->end() != branchSearchResult) {
    if ( branchSearchResult->second.type == UnlockMapOperation::UnlockMapType::BackEdge
      && branchSearchResult->second.destination == Instruction
    ) {
      return ForkAction{ForkActionType::Lock, branchSearchResult->second.exit};
    }
  }

  auto searchResult = trueForkMarkers.find(Instruction);
  if (trueForkMarkers.end() != searchResult) {
    return ForkAction{ForkActionType::ForkTrue};
  }

  searchResult = falseForkMarkers.find(Instruction);
  if (falseForkMarkers.end() != searchResult) {
    return ForkAction{ForkActionType::ForkFalse};
  }

  return ForkAction{ForkActionType::Keep};
}

Fact* Fact::getGeneratorUnit () const {
  return generatorUnit;
}

uint64_t Fact::getId () const {
  return ID;
}

const std::list<Fact::IntegerValueSnapshot>*
Fact::getIntegerSnapshots (const llvm::Instruction *Instruction) const {
  auto searchResult = zeroUnit->integerValueSnapshots.find(Instruction);

  if (zeroUnit->integerValueSnapshots.end() != searchResult) {
    return &(searchResult->second);
  } else {
    return nullptr;
  }
}

const std::set<Fact*>* Fact::getPassedGhosts (const llvm::Instruction *Instruction) const {
  auto searchResult = zeroUnit->ghostPassageMap.find(Instruction);

  if (zeroUnit->ghostPassageMap.end() != searchResult) {
    return &(searchResult->second);
  } else {
    return nullptr;
  }
}

const std::set<Fact*>* Fact::getPassedWindows (const llvm::Instruction *Instruction) const {
  auto searchResult = zeroUnit->windowPassageMap.find(Instruction);

  if (zeroUnit->windowPassageMap.end() != searchResult) {
    return &(searchResult->second);
  } else {
    return nullptr;
  }
}

Fact::Type Fact::getType () const {
  return type;
}

uint64_t Fact::getUnitCount () const {
  return firstUnit->nthUnit;
}

Fact* Fact::getZeroUnit () const {
  return zeroUnit;
}

void Fact::insertValueToSet (
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

void Fact::insertValue (const llvm::Value *value) {
  insertValueToSet(
      value
    , primaryValues
    , stackedValues
  );
}

void Fact::insertRankValue (const llvm::Value *value) {
  insertValueToSet(
      value
    , primaryRankValues
    , stackedRankValues
  );
}

bool Fact::knowsAddress (const llvm::Value *Value) const {
  bool knowsValue = knowsValueInSet(
      Value
    , primaryMemoryAddresses
    , stackedMemoryAddresses
    , {Fact::Type::Win}
  );

  if (!knowsValue && nullptr != previousUnit) {
    return previousUnit->knowsAddress(Value);
  } else {
    return knowsValue;
  }
}

bool Fact::knowsLocalAddress (const llvm::Value *Value) const {
  bool knowsValue = knowsValueInSet(
      Value
    , primaryLocalMemoryAddresses
    , stackedLocalMemoryAddresses
    , {Fact::Type::Win}
  );

  if (!knowsValue && nullptr != previousUnit) {
    return previousUnit->knowsLocalAddress(Value);
  } else {
    return knowsValue;
  }
}

bool Fact::knowsRankValue (const llvm::Value *Value) const {
  return knowsValueInSet(
      Value
    , primaryRankValues
    , stackedRankValues
    , {Fact::Type::Comm, Fact::Type::Null}
  );
};

bool Fact::knowsSharedAddress (const llvm::Value *Value) const {
  bool knowsValue = knowsValueInSet(
      Value
    , primarySharedMemoryAddresses
    , stackedSharedMemoryAddresses
    , {Fact::Type::Win}
  );

  if (!knowsValue && nullptr != previousUnit) {
    return previousUnit->knowsSharedAddress(Value);
  } else {
    return knowsValue;
  }
}

bool Fact::knowsCommunicator (const llvm::Value *Value) const {
  if (nullptr != communicator) {
    auto valueConstant = llvm::dyn_cast<llvm::ConstantInt>(Value);
    auto ownConstant = llvm::dyn_cast<llvm::ConstantInt>(communicator);

    if (valueConstant && ownConstant) {
      return valueConstant->getZExtValue() == ownConstant->getZExtValue();
    } else if (nullptr != generatorUnit) {
      return generatorUnit->knowsValue(Value);
    }
  }

  return false;
}

bool Fact::knowsValue (const llvm::Value *Value) const {
  bool knowsValue = knowsValueInSet(
      Value
    , primaryValues
    , stackedValues
    , {Fact::Type::Comm, Fact::Type::Win, Fact::Type::IntConst}
  );

  if (!knowsValue && nullptr != previousUnit) {
    return previousUnit->knowsValue(Value);
  } else {
    return knowsValue;
  }
}

bool Fact::knowsValueInSet (
    const llvm::Value *value
  , const Fact::ValueSet &primarySet
  , const Fact::StackedValueSet &stackedSet
  , const Fact::FactTypeSet &allowedTypes
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

void Fact::increaseUnitCount (uint64_t visitorCount) {
  firstUnit->nthUnit = std::max(visitorCount + 1, firstUnit->nthUnit);
}

void Fact::increaseWindowCounter () {
  currentWindow += 1;
}

bool Fact::isAncestor (const Fact *Other) const {
  if (this == Other) {
    return false;
  }

  for (auto ancestor : ancestors) {
    if (Other == ancestor) {
      return true;
    } else {
      if (ancestor->isAncestor(Other)) {
        return true;
      }
    }
  }

  return false;
}

bool Fact::isDroppedForForking () const {
  return hasBeenForked;
}

bool Fact::isLocked () const {
  return nullptr != lockUntil;
}

bool Fact::isLookingErrorneous () const {
  return nullptr != getPath()->getViolatingInstruction()
    || !getPath()->hasFinalState();
}

bool Fact::isMarkedForPurging () const {
  return markedForPurging;
}

bool Fact::isMutable () const {
  return !dontFork;
}

bool Fact::isStillForkable (const llvm::Instruction *Instruction) const {
  return forkDirectionSet.end() == forkDirectionSet.find(Instruction);
}

bool Fact::isType (const Type T) const {
  return T == type;
}

void Fact::lock (
    const llvm::Instruction *UntilInstruction
  , const llvm::Instruction *AtInstruction
) {
  lockUntil = UntilInstruction;
}

void Fact::markAsForkedByCondition (
    const llvm::Instruction *Instruction
  , bool trueBranch
) {
  if (!isType(Fact::Type::Win)) {
    return;
  }

  auto state = trueBranch
    ? WindowState::State::ForkTrue
    : WindowState::State::ForkFalse;

  getPath()->transit(state, Instruction, MPISymbol::MPIConditionAssertion);
}

void Fact::markIntegerUnresolvable () {
  constantValueLost = true;
}

void Fact::markForPurging (const llvm::Instruction *Instruction) {
  markedForPurging = true;

  if (nullptr != Instruction) {
    hasBeenForked = true;
    forkDirectionSet.insert(Instruction);
  }
}

void Fact::popTokenLayers () {
  if (!isLocked()) {
    if (stackedMemoryAddresses.size() > 0) {
      stackedMemoryAddresses.pop();
    }
    if (stackedLocalMemoryAddresses.size() > 0) {
      stackedLocalMemoryAddresses.pop();
    }
    if (stackedRankValues.size() > 0) {
      stackedRankValues.pop();
    }
    if (stackedSharedMemoryAddresses.size() > 0) {
      stackedSharedMemoryAddresses.pop();
    }
    if (stackedValues.size() > 0) {
      stackedValues.pop();
    }
  }
}

void Fact::pushTokenLayers () {
  if (!isLocked()) {
    stackedMemoryAddresses.push({});
    stackedLocalMemoryAddresses.push({});
    stackedRankValues.push({});
    stackedSharedMemoryAddresses.push({});
    stackedValues.push({});
  }
}

void Fact::registerGhostPassage (
    const llvm::Instruction *Instruction
  , Fact *Ghost
) {
  auto searchResult = zeroUnit->ghostPassageMap.find(Instruction);

  if (zeroUnit->ghostPassageMap.end() != searchResult) {
    searchResult->second.insert(Ghost);
  } else {
    zeroUnit->ghostPassageMap[Instruction] = {Ghost};
  }
}

void Fact::registerParallelWindowPassage (
    const llvm::Instruction *Instruction
  , Fact *Window
) {
  auto searchResult = zeroUnit->parallelWindowPassageMap.find(Instruction);

  if (zeroUnit->parallelWindowPassageMap.end() != searchResult) {
    searchResult->second.insert(Window);
  } else {
    zeroUnit->parallelWindowPassageMap[Instruction] = {Window};
  }
}

WindowPath* Fact::registerStateMachine () {
  firstUnit->stateMachines.emplace_back(std::make_unique<WindowPath>());

  return firstUnit->stateMachines.back().get();
}

WindowPath* Fact::registerStateMachine (WindowPath *otherStateMachine) {
  firstUnit->stateMachines.emplace_back(std::make_unique<WindowPath>(*otherStateMachine));

  return firstUnit->stateMachines.back().get();
}

void Fact::registerBranch (const llvm::BranchInst *BranchInst) {
  if (!isLocked()) {
    branchMarker = BranchInst;

    if (BranchInst->isConditional()) {
      auto firstBranchTarget = &*(BranchInst->getSuccessor(0)->begin());
      trueForkMarkers.insert(firstBranchTarget);

      auto secondBranchTarget = &*(BranchInst->getSuccessor(1)->begin());
      falseForkMarkers.insert(secondBranchTarget);
    }
  }
}

void Fact::registerWindowPassage (
    const llvm::Instruction *Instruction
  , Fact *Window
) {
  auto searchResult = zeroUnit->windowPassageMap.find(Instruction);

  if (zeroUnit->windowPassageMap.end() != searchResult) {
    searchResult->second.insert(Window);
  } else {
    zeroUnit->windowPassageMap[Instruction] = {Window};
  }
}

void Fact::setCurrentInteger (int64_t Value) {
  constantValueLost = false;
  constantValue = Value;
}

void Fact::setGenerationData (
    const llvm::Instruction *CreatingInstruction
  , const llvm::Value *CommuicatorValue
  , Fact *GeneratorUnit
) {
  creatingInstruction = CreatingInstruction;

  if (isType(Fact::Type::Win) && nullptr == communicator) {
    communicator = CommuicatorValue;
    generatorUnit = GeneratorUnit;

    localPath->transit(
        WindowState::State::Created
      , CreatingInstruction
      , MPISymbol::MPIWinCreateGeneric
    );
  } else if (isType(Fact::Type::WinGhost) && nullptr == generatorUnit) {
    generatorUnit = GeneratorUnit;
  }
}

void Fact::setWindowInformation (MPISymbol Symbol, const llvm::Value *InfoValue) {
  creatorSymbol = Symbol;
}

void Fact::snapshotIntegerValue (const llvm::Instruction *Instruction) {
  if (isType(Fact::Type::IntConst)) {
    zeroUnit->integerValueSnapshots[Instruction].emplace_back(
        this
      , constantValue
      , constantValueLost
    );
  }
}

std::string Fact::toString () const {
  if (Fact::Type::Null == type) {
    return "null";
  } else {
    std::stringstream ss;

    switch (type) {
      case Fact::Type::Comm:
        ss << "Comm";
        break;
      case Fact::Type::Win:
        ss << "Win";
        break;
      case Fact::Type::WinGhost:
        ss << "WinGhost";
        break;
      case Fact::Type::IntConst:
        ss << "Integer";
        break;
      default:
        break;
    }

    ss << " ("  << ID << "):";

    if (Fact::Type::WinGhost == type) {
      ss << " of " << getGeneratorUnit()->getId() << std::endl;
    } else if (Fact::Type::IntConst == type) {
      ss << " : ";

      if (constantValueLost) {
        ss << "✝";
      } else {
        ss << constantValue;
      }

      ss << " (possibly an end value)" << std::endl;
    } else {
      ss << std::endl;

      for (auto value : primaryValues) {
        ss << "  " << psr::llvmIRToString(value) << "; " << std::endl;
      }
    }

    return ss.str();
  }
}

void Fact::unlock () {
  if (isLocked()) {
    lockUntil = nullptr;
  }
}
