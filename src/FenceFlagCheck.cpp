#include <stack>

#include "mpi.h"

#include "llvm/IR/Constants.h"
#include "llvm/IR/Instructions.h"

#include "FenceFlagCheck.h"

#include <iostream>

FenceFlagCheck::FenceFlagCheck (const std::set<Fact*> &RankSet)
  : rankSet(RankSet)
{}

std::list<FenceFlagCheck::ResultItem> FenceFlagCheck::validate () const {
  std::list<FenceFlagCheck::ResultItem> results;
  FenceFlagCheck::ExecutionContext context;

  linearize(context);

  uint64_t totalFences = 0;

  // Given the deadlock test running before this, this expected to hold
  // for all facts.
  if (context.facts.size() > 0) {
    totalFences = context.facts.begin()->second.fenceHistory.size();
  }

  bool running = true;

  for (uint64_t i = 0; i < totalFences && running; ++i) {
    bool firstFlag = true;

    for (auto &pair : context.facts) {
      auto &execution = pair.second;

      auto callInstruction = llvm::dyn_cast<llvm::CallInst>(execution.fenceHistory[i]->instruction);

      if (auto flagArg = llvm::dyn_cast<llvm::ConstantInt>(callInstruction->getArgOperand(0))) {
        int64_t flag = flagArg->getSExtValue(), currentFlag = 0;

        if (flag & MPI_MODE_NOPRECEDE) {
          currentFlag |= MPI_MODE_NOPRECEDE;
        }
        if (flag & MPI_MODE_NOSUCCEED) {
          currentFlag |= MPI_MODE_NOSUCCEED;
        }

        if (firstFlag) {
          context.currentFlag = currentFlag;
          firstFlag = false;
        } else {
          if (context.currentFlag != currentFlag) {
            running = false;
            break;
          }
        }
      } else {
        // Currently, we cannot help.
        break;
      }
    }

    context.currentFlag = 0;
  }

  return results;
}

void FenceFlagCheck::linearize (
    FenceFlagCheck::ExecutionContext &context
) const {
  for (auto fact : rankSet) {
    context.facts[fact] = {};
    auto &list = context.facts[fact].fenceHistory;

    std::stack<const WindowPath*> paths;

    auto path = fact->getPath();
    const WindowPath *currentPath = path;

    while (nullptr != currentPath) {
      paths.push(currentPath);
      currentPath = currentPath->getPreviousPath();
    }

    while (paths.size() > 0) {
      auto path = paths.top();
      paths.pop();

      auto &transitions = path->getTransitions();

      for (auto it = transitions.begin(); it != transitions.end(); ++it) {
        auto symbol = it->symbol;

        switch(symbol) {
          case MPISymbol::MPIWinFence:
            list.emplace_back(&*it);
            break;
          default:
            break;
        }
      }
    }
  }
}
