#include "llvm/IR/CallSite.h"
#include "llvm/IR/Constants.h"

#include "CommSynchronizationCheck.h"

CommSynchronizationCheck::CommSynchronizationCheck (
    CommSynchronizationCheck::RankSet &rankSet
) : factSet(rankSet)
{}

/**
 * This test can look for deadlocking of two cases:
 *
 * * MPI_Barrier(MPI_COMM_WORLD) vs MPI_Win_fence.
 * * a mismatching no. of MPI_Win_fence.
 *
 */

std::list<CommSynchronizationCheck::ResultItem>
CommSynchronizationCheck::validate (
) const {
  std::list<ResultItem> list;
  Context context;

  linearize(context);

  bool keepRunning = true;

  while (keepRunning) {
    bool progress = false;

    for (auto &pair : context.facts) {
      auto &execution = pair.second;

      if (ExecutionState::Enabled == execution.state) {
        const auto historySize = execution.syncHistory.size();

        if (historySize == execution.index) {
          execution.state = ExecutionState::Done;
          continue;
        }
        progress = true;

        for (; execution.index < historySize; execution.index++) {
          auto transition = execution.syncHistory[execution.index];
          auto symbol = transition->symbol;

          if (MPISymbol::MPIWinFence == symbol) {
            execution.state = ExecutionState::FenceWaiting;
            context.fenceWaits += 1;
            break;
          } else if (MPISymbol::MPIBarrier == symbol) {
            if (auto callInst = llvm::dyn_cast<llvm::CallInst>(transition->instruction)) {
              llvm::ImmutableCallSite callSite(callInst);
              auto barrierValue = callSite.getArgument(0);

              bool acquiredBarrierWait = false;

              if (auto constant = llvm::dyn_cast<llvm::ConstantInt>(barrierValue)) {
                acquiredBarrierWait = true;
                context.barrierWaits += 1;
                execution.state = ExecutionState::BarrierWaiting;
              }

              // We could not obtain the constant comm. of that barrier or could not
              // assign it to the given window while there are already facts hanging
              // at a barrier. This must be unbroken then.
              if (!acquiredBarrierWait) {
                context.skipBarrierCheck = true;
              } else {
                break;
              }
            }
          }
        }
      }
    }

    bool allAtBarrier = context.facts.size() == context.barrierWaits;
    if (allAtBarrier || context.skipBarrierCheck) {
      context.barrierWaits = 0;
      progress = true;
      context.skipBarrierCheck = false;
    }

    bool allAtFence = context.facts.size() == context.fenceWaits;
    if (allAtFence) {
      context.fenceWaits = 0;
      progress = true;
    }

    for (auto &pair : context.facts) {
      auto &execution = pair.second;

      if (ExecutionState::BarrierWaiting == execution.state) {
        if (context.skipBarrierCheck || allAtBarrier) {
          execution.state = ExecutionState::Enabled;
          execution.index += 1;
        }
      } else if (ExecutionState::FenceWaiting == execution.state) {
        if (allAtFence) {
          execution.state = ExecutionState::Enabled;
          execution.index += 1;
        }
      }
    }

    context.fenceWaits = 0;

    if (!progress) {
      keepRunning = false;
    }
  }

  for (auto &pair : context.facts) {
    auto &execution = pair.second;

    if (ExecutionState::Done != execution.state) {
      auto &execution = pair.second;

      list.emplace_back(pair.first, execution.syncHistory[execution.index]->instruction);
    }
  }

  return list;
}

void CommSynchronizationCheck::linearize (
    CommSynchronizationCheck::Context &context
) const {
  for (auto fact : factSet) {
    context.facts[fact] = {};
    auto &list = context.facts[fact].syncHistory;

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
          case MPISymbol::MPIBarrier:
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
