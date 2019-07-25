#ifndef H_LAZY_FACT
#define H_LAZY_FACT

#include <set>
#include <stack>

#include "llvm/IR/Instructions.h"
#include "llvm/IR/Value.h"

#include "MPISymbols.h"
#include "UnlockMap.h"
#include "WindowState.h"

class LazyFact {
  public:
    /**
     * The type of the fact.
     *
     * * Null: Zero value, mostly used to store fact-invariant information.
     * * Comm: An explicit MPI comm handle, i.e. NOT `MPI_COMM_WORLD`.
     * * Win: An MPI window handle.
     */
    enum class Type { Null, Comm, Win };

    /**
     * For windows only: Set to define what to do on a forking edge:
     *
     * * Drop: Remove the fact from the set.
     * * Keep: Do nothing.
     * * Lock: Lock the window against forking up to reaching a given instruction.
     * * PrepareLoop: Just before hitting a loop head, create a new fact marking the new loop.
     * * Replace: Replace the fact over reaching a new state.
     * * ExitLoop: Replace the fact over exiting a loop.
     */
    enum class ForkActionType {
        Drop
      , Keep
      , Lock
      , PrepareLoop
      , PrepareDoLoop
      , ExitLoop
      , Replace
    };

    using FactTypeSet = std::set<Type>;

    using ValueSet = std::set<const llvm::Value*>;
    using StackedValueSet = std::stack<ValueSet>;

    /**
     * Just a bundling struct of `ForkActionType` and an associated instruction
     * in case of `Lock`: the instruction where to unlock on reaching.
     */
    struct ForkAction {
      ForkActionType actionType;
      const llvm::Instruction *instruction;

      ForkAction (ForkActionType T, const llvm::Instruction *I)
        : actionType(T), instruction(I) {}

      ForkAction (ForkActionType T) :
         actionType(T), instruction(nullptr) {}
    };

    LazyFact (Type T, const UnlockMap&, uint32_t ithWindow = 0);
    LazyFact (Type T, LazyFact*);
    LazyFact (
        const LazyFact&
      , const llvm::Instruction*
      , ForkActionType actionType = ForkActionType::Replace
      , MPISymbol symbol = MPISymbol::Null
    );

    LazyFact (const LazyFact&) = delete;
    LazyFact (LazyFact&&) = delete;

    LazyFact& operator= (const LazyFact&) = delete;

    void addKnownValue (const llvm::Value*, bool initial = false);
    std::list<const llvm::Value*> addSomeKnownAddress (
        const llvm::Value*
      , ValueSet&
      , StackedValueSet&
      , bool initial = false
    );
    bool awaitsLockAfter (const llvm::Instruction*) const;

    bool canCreateWindowFact () const;

    const std::set<const llvm::GetElementPtrInst*>* findOffsetInfo (const llvm::Value*);

    const llvm::Instruction* getCreatingInstruction () const;
    ForkAction getForkAction (const llvm::Instruction*);
    uint64_t getId () const;
    const LazyFact* getPreviousFact () const;
    WindowState& getState ();
    const WindowState& getState () const;
    Type getType () const;

    void increaseWindowCounter ();

    bool isDroppedForForking () const;
    bool isLocked () const;
    bool isLookingErrorneous () const;
    bool isMarkedForPurging () const;
    bool isMarkedForViolation () const;
    bool isType (const Type T) const;

    bool knowsValue (const llvm::Value*) const;

    void lock (const llvm::Instruction*);

    void markForPurging (const llvm::Instruction *Instr = nullptr);
    void markForViolation ();

    void popTokenLayers ();
    void pushTokenLayers ();

    void setGenerationData (
        const llvm::Instruction*
      , LazyFact*
    );
    void setWindowInformation (MPISymbol);

    std::string toString () const;
    void unlock ();

  private:
    /**
     * A pointer the null unit.
     */
    LazyFact * const zeroUnit;
    /**
     * For windows: the communicator fact.
     */
    LazyFact *generatorUnit;
    /**
     * For windows: the semantic unit before forking, i.e. the previous Win call.
     */
    const LazyFact *previousUnit;
    /**
     * For windows: the unit before forking, regardless of state change.
     */
    const LazyFact *previousFlowUnit;
    /**
     * For windows: The instruction when to unlock the window fact again, i.e. when the
     * fact is passing the given edge to the destination of `lockUntil`. If this
     * is `nullptr`, there is no lock set.
     */
    const llvm::Instruction *lockUntil;
    /**
     * For null unit: The global map that knows when to lock facts until when.
     */
    const UnlockMap *unlockMap;
    /**
     * For windows: The `MPI_Win_create` call instruction that created this one. Needed
     * for forks to re-register on passage to correctly assign ancestors to forks as well,
     * if this happens at some point later than forking.
     */
    const llvm::Instruction *creatingInstruction;
    /**
     * For windows: the internal MPI symbol for `creatingInstruction`.
     */
    MPISymbol creatorSymbol;
    /**
     * For windows: the symbol of the current function.
     */
    MPISymbol currentSymbol;

    /**
     * For windows: the associated state machine.
     */
    WindowState state;

    // The fact ID.
    const uint64_t ID;
    // The type.
    const Type type;
    // A flag to explicitly indicate the urge to delete this fact. Probably only required
    // do deal with some Phasar issue.
    bool markedForPurging;
    // Indicates that this fact ended up being forked.
    bool hasBeenForked;
    // When reaching a loop header the first time, do not lock in the prepared fact, only
    // on its children.
    bool preventLock;
    // flag wether this transition caused a violation
    bool isViolation;
    // Null unit: if `ithWindow` > 0, count up to reaching that i-th window creation.
    uint32_t ithWindow;
    uint32_t currentWindow;

    ValueSet primaryValues;
    StackedValueSet stackedValues;

    /**
     * For null unit: A map between a base pointer and all relevant offset constants
     * that are known to refer to relevant values. Used to work on arrays, structs.
     */
    std::map<
        const llvm::Value*
      , std::set<const llvm::GetElementPtrInst*>
    > offsetAssociations;

    std::stack<const llvm::BranchInst*> loopEntries;

    // A static variable to assign a new ID to each fact.
    static uint64_t instances;

    void insertValue (const llvm::Value*);

    void insertValueToSet (
        const llvm::Value*
      , ValueSet&
      , StackedValueSet&
    );

    bool knowsValueInSet (
        const llvm::Value*
      , const ValueSet&
      , const StackedValueSet&
      , const FactTypeSet&
    ) const;
};

#endif
