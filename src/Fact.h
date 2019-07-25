#ifndef H_FACT_UNIT
#define H_FACT_UNIT

#include <map>
#include <queue>
#include <set>
#include <stack>

#include "llvm/IR/Instructions.h"
#include "llvm/IR/Value.h"

#include "UnlockMap.h"
#include "WindowPath.h"

class Fact {
  public:
    /**
     * The type of the fact.
     *
     * * Null: Zero value, mostly used to store fact-invariant information.
     * * Comm: An explicit MPI comm handle, i.e. NOT `MPI_COMM_WORLD`.
     * * Win: An MPI window handle.
     * * WinGhost: A light-weight overleft of a freed window, use to manage
     *      ordering of windows over time.
     * * IntConst: A scalar integer inside the memory that is known to have
     *      a constant value.
     */
    enum class Type { Null, Comm, Win, WinGhost, IntConst };

    /**
     * For windows only: Set to define what to do on a forking edge:
     *
     * * Drop: Remove the fact from the set.
     * * Fork: Create a fork of the object.
     * * ForkRankFalse: Create a fork of the object over the negative rank comparison.
     * * ForkRankTrue: Create a fork of the object over the positive rank comparison.
     * * Keep: Do nothing.
     * * Lock: Lock the window against forking up to reaching a given instruction.
     * * Unlock: Remove the lock.
     */
    enum class ForkActionType {
        Drop
      , ForkFalse
      , ForkTrue
      , Keep
      , Lock
      , Unlock
    };

    using CallValueMap = std::map<const llvm::Instruction*, std::set<const llvm::Value*>>;
    using ValueCallMap = std::map<const llvm::Value*, const llvm::Instruction*>;

    using FactTypeSet = std::set<Type>;

    using ValueSet = std::set<const llvm::Value*>;
    using StackedValueSet = std::stack<ValueSet>;

    using PassageMap = std::map<const llvm::Instruction*, std::set<Fact*>>;

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

    /**
     * The struct to be used at each instruction to capture the current state
     * of every living integer value there.
     */
    struct IntegerValueSnapshot {
      const Fact * const fact;
      const int64_t value;
      const bool isLost;

      IntegerValueSnapshot (Fact *F, int64_t Value, bool Lost)
        : fact(F), value(Value), isLost(Lost)
      {}
    };

    Fact (Type T, const UnlockMap&, uint32_t ithWindow = 0);
    Fact (Type T, Fact*, int64_t = 0, bool dontFork = false);
    Fact (const Fact&, const llvm::Instruction*);

    Fact (const Fact&) = delete;
    Fact (Fact&&) = delete;

    Fact& operator= (const Fact&) = delete;

    void addAncestry (const Fact*);
    void addKnownAddress (const llvm::Value*, bool initial = false);
    void addKnownInitialLocalAddress (const llvm::Value*, const llvm::Instruction*);
    void addKnownLocalAddress (const llvm::Value*, const llvm::Value*);
    void addKnownInitialSharedAddress (const llvm::Value*, const llvm::Instruction*);
    void addKnownSharedAddress (const llvm::Value*, const llvm::Value*);
    void addKnownRankValue (const llvm::Value*, bool initial = false);
    void addKnownValue (const llvm::Value*, bool initial = false);

    bool canCreateWindowFact () const;

    const std::set<const llvm::GetElementPtrInst*>* findOffsetInfo (const llvm::Value*);

    const llvm::BranchInst* getBranchMarker () const;
    const CallValueMap& getCallValueMap () const;
    const llvm::Value* getCommunicator () const;
    MPISymbol getCreatorSymbol () const;
    int64_t getCurrentInteger () const;
    ForkAction getForkAction (const llvm::Instruction*) const;
    Fact* getGeneratorUnit () const;
    uint64_t getId () const;
    const std::list<IntegerValueSnapshot>* getIntegerSnapshots (const llvm::Instruction*) const;
    const std::set<Fact*>* getPassedGhosts (const llvm::Instruction*) const;
    const std::set<Fact*>* getPassedWindows (const llvm::Instruction*) const;
    WindowPath* getPath () const;
    Type getType () const;
    uint64_t getUnitCount () const;
    const ValueCallMap& getValueCallMap () const;
    Fact* getZeroUnit () const;

    void increaseUnitCount (uint64_t);
    void increaseWindowCounter ();

    bool isAncestor (const Fact*) const;
    bool isDroppedForForking () const;
    bool isLocked () const;
    bool isLookingErrorneous () const;
    bool isMarkedForPurging () const;
    bool isMutable () const;
    bool isStillForkable (const llvm::Instruction*) const;
    bool isType (const Type T) const;

    bool knowsAddress (const llvm::Value*) const;
    bool knowsCommunicator (const llvm::Value*) const;
    bool knowsLocalAddress (const llvm::Value*) const;
    bool knowsRankValue (const llvm::Value*) const;
    bool knowsSharedAddress (const llvm::Value*) const;
    bool knowsValue (const llvm::Value*) const;

    void lock (const llvm::Instruction*, const llvm::Instruction*);

    void markAsForkedByCondition (const llvm::Instruction*, bool);
    void markForPurging (const llvm::Instruction *Instr = nullptr);
    void markIntegerUnresolvable ();

    void popTokenLayers ();
    void pushTokenLayers ();

    void registerBranch (const llvm::BranchInst*);
    void registerGhostPassage (const llvm::Instruction*, Fact*);
    void registerJump (const llvm::BranchInst*);
    void registerParallelWindowPassage (const llvm::Instruction*, Fact*);
    void registerWindowPassage (const llvm::Instruction*, Fact*);

    void setCurrentInteger (int64_t);
    void setGenerationData (
        const llvm::Instruction*
      , const llvm::Value*
      , Fact*
    );
    void setWindowInformation (MPISymbol, const llvm::Value*);

    void snapshotIntegerValue (const llvm::Instruction*);

    std::string toString () const;
    void unlock ();

  private:
    /**
     * For the windows: the first, non-forked window fact for any child.
     */
    Fact * const firstUnit;
    /**
     * A pointer the null unit.
     */
    Fact * const zeroUnit;
    /**
     * For windows: the value of the communicator that we found in the given
     * `MPI_Win_create` call instruction. Used to match different facts via their
     * generating comm fact.
     */
    const llvm::Value *communicator;
    /**
     * For windows: the communicator fact.
     * For ghosts: the ancestor window fact.
     */
    Fact *generatorUnit;
    /**
     * For windows/integers: the unit before forking.
     */
    const Fact *previousUnit;
    /**
     * For windows: The `MPI_Win_create` call instruction that created this one. Needed
     * for forks to re-register on passage to correctly assign ancestors to forks as well,
     * if this happens at some point later than forking.
     */
    const llvm::Instruction *creatingInstruction;
    /**
     * For windows: The path associated to this fact, to be used for symbolic
     * execution.
     */
    WindowPath *localPath;
    /**
     * For windows: A minor pointer to correctly create the fork history from the parent
     * fact that noticed this branch instruction.
     */
    const llvm::BranchInst *branchMarker;
    /**
     * For windows, ints: The instruction when to unlock the window fact again, i.e. when the
     * fact is passing the given edge to the destination of `lockUntil`. If this
     * is `nullptr`, there is no lock set.
     */
    const llvm::Instruction *lockUntil;
    /**
     * For null unit: The global map that knows when to lock facts until when.
     */
    const UnlockMap *unlockMap;
    /**
     * For windows: the internal MPI symbol for `creatingInstruction`.
     */
    MPISymbol creatorSymbol;

    // The fact ID.
    const uint64_t ID;
    // The type.
    const Type type;
    // Int Const: don't fork, but also don't do updates then, assuming
    // the rank values will never change, and we don't take for everything else
    const bool dontFork;
    // A flag to explicitly indicate the urge to delete this fact. Probably only required
    // do deal with some Phasar issue.
    bool markedForPurging;
    // Indicates that this fact ended up being forked.
    bool hasBeenForked;
    // A value to track being the n-th instance on the path.
    uint64_t nthUnit;
    // IntConst: the current constant value
    int64_t constantValue;
    // IntConst: unresolvable
    bool constantValueLost;
    // Null unit: if `ithWindow` > 0, count up to reaching that i-th window creation.
    uint32_t ithWindow;
    uint32_t currentWindow;

    // Window, IntConst: remembers the directions (instructions) where we forked to.
    std::set<const llvm::Instruction*> forkDirectionSet;

    /**
     * For windows and communicators: LLVM values that can be identified as belonging
     * to either handle.
     *
     * `primaryValue`: Set of values generated by the creating stack-depth or global
     *    values. Can be more than one stack-level if e.g. a window has been created
     *    inside of a call and then returns to its parents.
     *
     * `secondaryValue`: Stack of value sets, one per call-stack level. Used to easily
     *    trash values that are no longer relevant.
     */
    ValueSet primaryValues;
    StackedValueSet stackedValues;

    /**
     * For windows: anything that looks like memory that belongs to the window.
     */
    ValueSet primaryMemoryAddresses;
    StackedValueSet stackedMemoryAddresses;

    /**
     * For windows: memory that is used to store window operations into or to
     *   read from when writing.
     */
    ValueSet primaryLocalMemoryAddresses;
    StackedValueSet stackedLocalMemoryAddresses;

    /**
     * Rich-faceted matter:
     *
     * If we are dealing with rank calls on `MPI_COMM_WORLD`, we possibly don't
     * have windows yet. But also, since we may encounter this on a window when
     * `Null` has not been there yet, we use windows to forward such values to
     * to the null unit.
     *
     * Otherwise, if we have explicit comm handles, this is taken by the communicator
     * fact.
     */
    ValueSet primaryRankValues;
    StackedValueSet stackedRankValues;

    /**
     * For windows: anything that looks like memory that belongs some other window
     * via shared memory access.
     */
    ValueSet primarySharedMemoryAddresses;
    StackedValueSet stackedSharedMemoryAddresses;

    /**
     * For windows: For each window call to something that reads from or writes to
     *   local memory, we collect every value that is derived from that call.
     *   For any later copy, we use the previous calue to do a lookup for the
     *   original generator call.
     *
     *   Used together with shared memory operations, as they should not overlap
     *   on a per-fact basis.
     */
    CallValueMap affectedLocalMemoryValues;
    ValueCallMap localMemoryGenerators;

    /**
     * For zero unit: A map that collects all `IntConst`s and their value at that
     * time for each instruction.
     */
    std::map<const llvm::Instruction*, std::list<std::pair<Fact*, int64_t>>> integerConstants;

    /**
     * For windows: For the first unit (i.e. not branch-children), this is the lifetime
     * management set of the intial SM and all forked state machines.
     */
    std::list<std::unique_ptr<WindowPath>> stateMachines;

    /**
     * For windows: A set of instructions that work as entry instruction for BBs on an
     * upcoming fork. Seperated into first (true) and second (false) sets.
     */
    std::set<const llvm::Instruction*> trueForkMarkers;
    std::set<const llvm::Instruction*> falseForkMarkers;

    /**
     * For windows: A set of window facts containing windows that safely belong *before*
     * this one in terms of the CFG.
     */
    std::set<const Fact*> ancestors;

    /**
     * For null unit: A map between a base pointer and all relevant offset constants
     * that are known to refer to relevant values. Used to work on arrays, structs.
     */
    std::map<
        const llvm::Value*
      , std::set<const llvm::GetElementPtrInst*>
    > offsetAssociations;

    /**
     * For null unit: a global lookup to connect ghosts and new window facts.
     *
     * `windowPassageMap` contains all newly created window facts (and their
     * forks) at a `MPI_Win_create...` call instruction.
     *
     * `parallelWindowPassageMap` contains a map of all windows that co-existed
     * actively at the point of creating a new window.
     *
     * `ghostPassageMap` contains all the ghosts that went through here.
     *
     * We need this to associate the ancestors' ghosts to windows that eventually
     * must have met at this point, regardless of where IFDS schedules either
     * to actually pass this call.
     */
    PassageMap parallelWindowPassageMap;
    PassageMap windowPassageMap;
    PassageMap ghostPassageMap;

    /**
     * For null unit: An index that snapshots the value of each alive integer at
     * passing the certain instruction.
     */
    std::map<const llvm::Instruction*, std::list<IntegerValueSnapshot>> integerValueSnapshots;

    // A static variable to assign a new ID to each fact.
    static uint64_t instances;

    std::list<const llvm::Value*> addSomeKnownAddress (
        const llvm::Value*
      , ValueSet&
      , StackedValueSet&
      , bool initial = false
    );

    void insertValue (const llvm::Value*);
    void insertRankValue (const llvm::Value*);

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

    WindowPath* registerStateMachine ();
    WindowPath* registerStateMachine (WindowPath*);
};

#endif
