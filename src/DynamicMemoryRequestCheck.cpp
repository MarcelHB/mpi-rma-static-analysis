#include <stack>

#include "DynamicMemoryRequestCheck.h"

DynamicMemoryRequestCheck::DynamicMemoryRequestCheck (const Fact *Window)
  : window(Window)
{}

const llvm::Instruction* DynamicMemoryRequestCheck::validate () const {
  MPISymbol windowCreator = window->getCreatorSymbol();

  std::stack<const WindowPath*> paths;
  const WindowPath* currentPath = window->getPath();

  while (nullptr != currentPath) {
    paths.push(currentPath);
    currentPath = currentPath->getPreviousPath();
  }

  while (paths.size() > 0) {
    auto path = paths.top();
    paths.pop();

    auto &transitions = path->getTransitions();

    for (auto it = transitions.begin(); it != transitions.end(); ++it) {
      auto currentSymbol = it->symbol;

      if (MPISymbol::MPIWinCreateDynamic != windowCreator) {
        if (  MPISymbol::MPIWinAttach == currentSymbol
          ||  MPISymbol::MPIWinDetach == currentSymbol
        ) {
          return it->instruction;
        }
      } else if (MPISymbol::MPIWinAllocateShared != windowCreator) {
        if (MPISymbol::MPIWinSharedQuery == currentSymbol) {
          return it->instruction;
        }
      }
    }
  }

  return nullptr;
}
