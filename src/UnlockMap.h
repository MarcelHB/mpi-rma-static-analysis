#ifndef H_UNLOCK_MAP
#define H_UNLOCK_MAP

#include <list>
#include <map>
#include <utility>

#include "llvm/IR/Instruction.h"

struct UnlockMapOperation {
  enum class UnlockMapType { BackEdge, Forward, ForwardInDo };

  const llvm::Instruction *destination;
  const llvm::Instruction *exit;
  UnlockMapType type;

  UnlockMapOperation ()
    : destination(nullptr)
    , exit(nullptr)
    , type(UnlockMapType::BackEdge)
  {}

  UnlockMapOperation (
      const llvm::Instruction *Destination
    , const llvm::Instruction *Exit
    , UnlockMapType T
  ) : destination(Destination)
    , exit(Exit)
    , type(T)
  {}
};

using UnlockMap =
  std::map<
    const llvm::Instruction*
  , UnlockMapOperation
  >;

#endif
