#ifndef H_COMM_SYNCHRONIZATION_CHECK
#define H_COMM_SYNCHRONIZATION_CHECK

#include <set>
#include <vector>

#include "Fact.h"
#include "WindowPath.h"

class CommSynchronizationCheck {
  public:
    using RankSet = std::set<Fact*>;

    CommSynchronizationCheck (RankSet&);

    struct ResultItem {
      Fact *fact;
      const llvm::Instruction *instruction;

      ResultItem (Fact *Fact, const llvm::Instruction *Instruction)
        : fact(Fact), instruction(Instruction) {}
    };

    std::list<ResultItem> validate () const;
  private:
    enum class ExecutionState { Enabled, BarrierWaiting, FenceWaiting, Done };

    struct Execution {
      std::vector<const WindowPath::Transition*> syncHistory;
      uint64_t index;
      ExecutionState state;

      Execution () : index(0), state(ExecutionState::Enabled) {}
    };

    struct Context {
      std::map<Fact*, Execution> facts;
      bool skipBarrierCheck;
      uint64_t fenceWaits;
      uint64_t barrierWaits;

      Context () :
          skipBarrierCheck(false)
        , fenceWaits(0)
        , barrierWaits(0)
      {}
    };

    RankSet &factSet;

    void linearize (Context&) const;
};

#endif
