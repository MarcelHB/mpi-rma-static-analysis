#ifndef H_MPI_SYMBOLS
#define H_MPI_SYMBOLS

enum class MPISymbol {
    Null = 0
  , MPIConditionAssertion
  , MPIWinCreateGeneric
  , MPIDataRead
  , MPIDataWrite
  , MPILocalRead
  , MPILocalWrite
  , MPIOtherFence
  , MPISharedRead
  , MPISharedWrite
  // actual call aliases below
  , MPIAccumulate
  , MPIBarrier
  , MPICommFree
  , MPICompareAndSwap
  , MPIGet
  , MPIGetAccumulate
  , MPIFetchAndOp
  , MPIPut
  , MPIRaccumulate
  , MPIRget
  , MPIRgetAccumulate
  , MPIRput
  , MPIWinAllocate
  , MPIWinAllocateShared
  , MPIWinAttach
  , MPIWinComplete
  , MPIWinCreate
  , MPIWinCreateDynamic
  , MPIWinDetach
  , MPIWinFence
  , MPIWinFlush
  , MPIWinFlushLocal
  , MPIWinFlushAll
  , MPIWinFlushLocalAll
  , MPIWinFree
  , MPIWinLock
  , MPIWinLockAll
  , MPIWinPost
  , MPIWinSharedQuery
  , MPIWinStart
  , MPIWinSync
  , MPIWinTest
  , MPIWinUnlock
  , MPIWinUnlockAll
  , MPIWinWait
};

#endif
