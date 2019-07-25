#include "UnsafeAccessCheck.h"

UnsafeAccessCheck::UnsafeAccessCheck (const Fact *Window)
  : window(Window)
{}

const llvm::Instruction* UnsafeAccessCheck::validate () const {
  std::stack<const WindowPath*> paths;
  const WindowPath* currentPath = window->getPath();

  while (nullptr != currentPath) {
    paths.push(currentPath);
    currentPath = currentPath->getPreviousPath();
  }

  auto state = UnsafeAccessCheck::State::Synchronized
    ,  nextState = UnsafeAccessCheck::State::Synchronized;
  std::set<const llvm::Instruction*> workingInstructions;

  while (paths.size() > 0) {
    auto path = paths.top();
    paths.pop();

    auto &transitions = path->getTransitions();

    for (auto it = transitions.begin(); it != transitions.end(); ++it) {
      auto symbol = it->symbol;
      bool actualCommCall = false;

      switch (symbol) {
        case MPISymbol::MPIAccumulate:
        case MPISymbol::MPICompareAndSwap:
        case MPISymbol::MPIFetchAndOp:
        case MPISymbol::MPIGet:
        case MPISymbol::MPIGetAccumulate:
        case MPISymbol::MPIPut:
          actualCommCall = true;
          nextState = UnsafeAccessCheck::State::Working;
          break;
        // TODO: Rget, Rput, ... they likely need additional request scanning
        case MPISymbol::MPIWinComplete:
        case MPISymbol::MPIWinFence:
        case MPISymbol::MPIWinFlush: // a bit unspecific for now
        case MPISymbol::MPIWinFlushLocal:
        case MPISymbol::MPIWinFlushAll:
        case MPISymbol::MPIWinFlushLocalAll:
          nextState = UnsafeAccessCheck::State::Synchronized;
          break;
        default:
          break;
      }

      if (  UnsafeAccessCheck::State::Synchronized == nextState
        &&  UnsafeAccessCheck::State::Working == state
      ) {
        workingInstructions.clear();
      } else if (
            UnsafeAccessCheck::State::Working == nextState
        &&  actualCommCall
      ) {
        workingInstructions.insert(it->instruction);
      }

      state = nextState;

      if (UnsafeAccessCheck::State::Synchronized == state) {
        continue;
      }

      if ( MPISymbol::MPIDataRead == symbol
        || MPISymbol::MPIDataWrite == symbol
      ) {
        const llvm::Instruction *accessInstruction = it->instruction;
        const llvm::Value *lookupValue = nullptr;

        // Warning: If there is ever more to handle than IR load/store here,
        // the whole path entry conepts needs further refinement into
        // IRSymbols or similar.
        if (MPISymbol::MPIDataRead == symbol) {
          const llvm::LoadInst *loadInstruction = llvm::dyn_cast<llvm::LoadInst>(accessInstruction);
          lookupValue = loadInstruction->getPointerOperand();
        } else {
          const llvm::StoreInst *storeInstruction = llvm::dyn_cast<llvm::StoreInst>(accessInstruction);
          lookupValue = storeInstruction->getPointerOperand();
        }

        auto &localOperationMemory = window->getCallValueMap();

        for (auto instruction : workingInstructions) {
          auto searchResult = localOperationMemory.find(instruction);

          if (localOperationMemory.end() == searchResult) {
            continue;
          }

          auto valueSearchResult = searchResult->second.find(lookupValue);

          if (searchResult->second.end() != valueSearchResult) {
            return accessInstruction;
          }
        }
      }
    }
  }

  return nullptr;
};
