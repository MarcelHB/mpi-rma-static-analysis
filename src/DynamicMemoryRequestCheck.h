#ifndef H_DYNAMIC_MEMORY_REQUEST_CHECK
#define H_DYNAMIC_MEMORY_REQUEST_CHECK

#include "llvm/IR/Instruction.h"

#include "Fact.h"

/* A fairly easy check: for anything but `MPI_Win_create_dynamic` windows,
 * calls to `MPI_Win_attach` or `MPI_Win_detach` are forbidden.
 */

class DynamicMemoryRequestCheck {
  public:
    DynamicMemoryRequestCheck (const Fact*);

    const llvm::Instruction* validate () const;

  private:
    const Fact * const window;
};

#endif
