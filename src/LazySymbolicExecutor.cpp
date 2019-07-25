#ifdef WITH_SIGNAL
#include <signal.h>
#endif

#ifdef WITH_MPI
#include <mpi.h>
#endif

#include "llvm/IR/Constants.h"

#include "LazySymbolicExecutor.h"

LazySymbolicExecutor::LazySymbolicExecutor (
    const llvm::CallInst *CallInstruction
  , MPISymbol Symbol
  , LazyMPIRMAUseProblem &ProblemInstance
) : callInstruction(CallInstruction)
  , windowPos(getWindowPositionFromMPISymbol(Symbol))
  , windowValue(CallInstruction->getArgOperand(windowPos))
  , symbol(Symbol)
  , problemInstance(ProblemInstance)
{}

std::set<LazyFact*> LazySymbolicExecutor::computeTargets (LazyFact *Value) {
  if (  Value->isType(LazyFact::Type::Win)
    &&  !Value->isLocked()
  ) {
    auto state = WindowState::State::Fence;

    if (Value->knowsValue(windowValue)) {
      switch (symbol) {
        case MPISymbol::MPIWinFence:
#ifdef WITH_MPI
          {
            const llvm::Value *assertionValue = callInstruction->getArgOperand(0);

            if (auto constant = llvm::dyn_cast<llvm::ConstantInt>(assertionValue)) {
              int64_t constantValue = constant->getSExtValue();

              if ((constantValue & (MPI_MODE_NOPRECEDE|MPI_MODE_NOSUCCEED)) == (MPI_MODE_NOPRECEDE|MPI_MODE_NOSUCCEED)) {
                state = WindowState::State::NoPreceedSucceedFence;
              } else if (constantValue & MPI_MODE_NOPRECEDE) {
                state = WindowState::State::NoPreceedFence;
              } else if (constantValue & MPI_MODE_NOSUCCEED) {
                state = WindowState::State::NoSucceedFence;
              }
            }
          }
#endif
          break;
        case MPISymbol::MPIAccumulate:
        case MPISymbol::MPICompareAndSwap:
        case MPISymbol::MPIGet:
        case MPISymbol::MPIGetAccumulate:
        case MPISymbol::MPIFetchAndOp:
        case MPISymbol::MPIPut:
          state = WindowState::State::RemoteCommunication;
          break;
        case MPISymbol::MPIRaccumulate:
        case MPISymbol::MPIRget:
        case MPISymbol::MPIRgetAccumulate:
        case MPISymbol::MPIRput:
          state = WindowState::State::RequestRemoteCommunication;
          break;
        case MPISymbol::MPIWinAttach:
        case MPISymbol::MPIWinDetach:
          state = WindowState::State::Resizing;
          break;
        case MPISymbol::MPIWinComplete:
          state = WindowState::State::Complete;
          break;
        case MPISymbol::MPIWinFlush:
        case MPISymbol::MPIWinFlushAll:
        case MPISymbol::MPIWinFlushLocal:
        case MPISymbol::MPIWinFlushLocalAll:
          state = WindowState::State::Flush;
          break;
        case MPISymbol::MPIWinLock:
          state = WindowState::State::Lock;
          break;
        case MPISymbol::MPIWinLockAll:
          state = WindowState::State::LockAll;
          break;
        case MPISymbol::MPIWinPost:
          state = WindowState::State::Post;
          break;
        case MPISymbol::MPIWinSharedQuery:
          state = WindowState::State::SharedQuery;
          break;
        case MPISymbol::MPIWinStart:
          state = WindowState::State::Start;
          break;
        case MPISymbol::MPIWinSync:
          state = WindowState::State::Sync;
          break;
        case MPISymbol::MPIWinUnlock:
          state = WindowState::State::Unlock;
          break;
        case MPISymbol::MPIWinUnlockAll:
          state = WindowState::State::UnlockAll;
          break;
        case MPISymbol::MPIWinTest:
        case MPISymbol::MPIWinWait:
          state = WindowState::State::TestWait;
          break;
        default:
          return {Value};
      }

      LazyFact* newFact = problemInstance.makeInitialFact(
          *Value
        , callInstruction
        , LazyFact::ForkActionType::Replace
        , symbol
      );

      Value->markForPurging(callInstruction);

#ifdef WITH_SIGNAL
      raise(SIGUSR2);
#endif

      auto &SM = newFact->getState();
      if (SM.transit(state)) {
#ifdef WITH_SIGNAL
        raise(SIGUSR1);
#endif
        return {newFact};
      } else {
        newFact->markForViolation();

        return {};
      }
    }
  }

  return {Value};
}

unsigned LazySymbolicExecutor::getWindowPositionFromMPISymbol (
  const MPISymbol Symbol
) {
  switch (Symbol) {
    case MPISymbol::MPIGetAccumulate:
    case MPISymbol::MPIRgetAccumulate:
      return 11;
    case MPISymbol::MPIAccumulate:
    case MPISymbol::MPIRaccumulate:
      return 8;
    case MPISymbol::MPIGet:
    case MPISymbol::MPIPut:
    case MPISymbol::MPIRget:
    case MPISymbol::MPIRput:
      return 7;
    case MPISymbol::MPICompareAndSwap:
    case MPISymbol::MPIFetchAndOp:
      return 6;
    case MPISymbol::MPIWinLock:
      return 3;
    case MPISymbol::MPIWinPost:
    case MPISymbol::MPIWinStart:
      return 2;
    case MPISymbol::MPIWinFence:
    case MPISymbol::MPIWinFlush:
    case MPISymbol::MPIWinFlushLocal:
    case MPISymbol::MPIWinLockAll:
    case MPISymbol::MPIWinUnlock:
      return 1;
    case MPISymbol::MPIWinComplete:
    case MPISymbol::MPIWinFlushAll:
    case MPISymbol::MPIWinFlushLocalAll:
    case MPISymbol::MPIWinSync:
    case MPISymbol::MPIWinTest:
    case MPISymbol::MPIWinUnlockAll:
    case MPISymbol::MPIWinWait:
    default:
      return 0;
  }
}


