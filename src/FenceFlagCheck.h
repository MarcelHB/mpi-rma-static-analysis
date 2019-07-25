#ifndef H_FENCE_FLAG_CHECK
#define H_FENCE_FLAG_CHECK

#include <set>
#include <vector>

#include "llvm/IR/Instruction.h"

#include "Fact.h"

class FenceFlagCheck {
  public:
    struct ResultItem {
      Fact *window;
      const llvm::Instruction *instruction;

      ResultItem (Fact *Window, const llvm::Instruction *Instruction)
        : window(Window), instruction(Instruction) {}
    };

    FenceFlagCheck (const std::set<Fact*>&);

    std::list<ResultItem> validate () const;

  private:
    struct Execution {
      std::vector<const WindowPath::Transition*> fenceHistory;
      uint64_t index;

      Execution () : index(0) {}
    };

    struct ExecutionContext {
      std::map<Fact*, Execution> facts;
      uint64_t currentFlag;

      ExecutionContext () : currentFlag(0) {}
    };

    const std::set<Fact*> &rankSet;

    void linearize (ExecutionContext&) const;
};


#endif
