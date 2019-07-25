#ifndef H_WINDOW_STATE
#define H_WINDOW_STATE

#include <cstdint>

class WindowState {
  public:
    enum class State {
        None
      , Complete
      , Created
      , Fence
      , Flush
      , Freed
      , Lock
      , LockAll
      , LockAndAll
      , NoPreceedFence
      , NoPreceedSucceedFence
      , NoSucceedFence
      , Post
      , PostStart
      , RemoteCommunication
      , RemoteCommunicationFence
      , RemoteCommunicationLock
      , RemoteCommunicationLockAll
      , RemoteCommunicationLockAndAll
      , RemoteCommunicationGATS
      , RemoteCommunicationGATS_PS
      , RequestRemoteCommunication
      , Start
      , Sync
      , TestWait
      , Unlock
      , UnlockAll
      // Soft states, i.e. never violating on the fly.
      , Barrier
      , BranchSync
      , CommKill
      , ForkTrue
      , ForkFalse
      , LocalRead
      , LocalWrite
      , OtherFence
      , Resizing
      , SharedQuery
    };

    WindowState ();

    bool hasFinalState () const;

    bool transit (const State state);
  private:
    State currentState;
    uint64_t lockCount;
    bool hadSecondFence;
};

#endif
