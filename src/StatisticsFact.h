#ifndef H_STATISTICS_FACT
#define H_STATISTICS_FACT

#include <map>
#include <set>
#include <stack>

#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"

#include "Settings.h"
#include "UnlockMap.h"

class StatisticsFact {
  public:
    enum class Type { NF, CF, CRF, RF, SF };

    struct Record {
      uint64_t numInstructions;
      uint64_t numInstructionsTotal;
      uint64_t numBranches;
      uint64_t numWindows;
      uint64_t numIntegers;
      uint64_t numWinCalls;

      Record ()
        : numInstructions(0)
        , numInstructionsTotal(0)
        , numBranches(0)
        , numWindows(0)
        , numIntegers(0)
        , numWinCalls(0)
      {}
    };

    StatisticsFact (const UnlockMap&, Settings::IntegerMode);

    void consume (
        const llvm::Instruction*
      , Type type
      , const llvm::Function* = nullptr
    );

    void finalize ();

    const Record& getRecord () const;

    void popLayer ();

  private:
    using ValueSet = std::set<const llvm::Value*>;
    using StackedValueSet = std::stack<ValueSet>;

    const llvm::Instruction *wormholingUntil;
    const UnlockMap &unlockMap;
    Record record;
    bool silent;
    Settings::IntegerMode integerMode;

    // See equivalents in `Fact`.
    std::map<
        const llvm::Value*
      , std::set<const llvm::GetElementPtrInst*>
    > offsetAssociations;

    ValueSet primaryValues;
    StackedValueSet stackedValues;

    void addKnownValue (const llvm::Value*, bool initial = false);

    void insertValue (const llvm::Value*);
    void insertValueToSet (
        const llvm::Value*
      , ValueSet&
      , StackedValueSet&
    );

    bool isForkingBack (const llvm::BranchInst*) const;

    bool knowsValue (const llvm::Value*) const;
    bool knowsValueInSet (
        const llvm::Value*
      , const ValueSet&
      , const StackedValueSet&
    ) const;

    void pushLayer ();
};

#endif
