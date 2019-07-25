#ifndef H_UNSAFE_ACCESS_CHECK
#define H_UNSAFE_ACCESS_CHECK

#include "llvm/IR/Instruction.h"

#include "Fact.h"

class UnsafeAccessCheck {
  public:
    UnsafeAccessCheck (const Fact*);

    const llvm::Instruction* validate () const;

  private:
    enum class State { Synchronized, Working };

    const Fact *window;
};

#endif
