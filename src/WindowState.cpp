#include "WindowState.h"

WindowState::WindowState ()
  : currentState(WindowState::State::None)
  , lockCount(0)
  , hadSecondFence(false)
{}

bool WindowState::hasFinalState () const {
  return State::Freed == currentState && 0 == lockCount;
}

bool WindowState::transit (const State NextState) {
  if (  State::Barrier == NextState
    ||  State::BranchSync == NextState
    ||  State::CommKill == NextState
    ||  State::ForkTrue == NextState
    ||  State::ForkFalse == NextState
    ||  State::LocalRead == NextState
    ||  State::LocalWrite == NextState
    ||  State::OtherFence == NextState
    ||  State::Resizing == NextState
    ||  State::SharedQuery == NextState
  ) {
    return true;
  }

  switch (NextState) {
    case State::Complete:
      if (  State::PostStart == currentState
        ||  State::RemoteCommunicationGATS_PS == currentState
      ) {
        currentState = State::Post;
      } else if (
            State::Start == currentState
        ||  State::RemoteCommunicationGATS == currentState
      ) {
        currentState = NextState;
      } else {
        return false;
      }

      break;
    case State::Created:
      if (State::None == currentState) {
        currentState = NextState;
      } else {
        return false;
      }

      break;
    case State::Fence:
      if (  State::Complete == currentState
        ||  State::Created == currentState
        ||  State::Fence == currentState
        ||  State::NoPreceedFence == currentState
        ||  State::NoPreceedSucceedFence == currentState
        ||  State::NoSucceedFence == currentState
        ||  State::RemoteCommunicationFence == currentState
        ||  State::TestWait == currentState
      ) {
        if (  State::Fence == currentState
          ||  State::NoPreceedFence == currentState
          ||  State::NoPreceedSucceedFence == currentState
          ||  State::NoSucceedFence == currentState
        ) {
          hadSecondFence = true;
        }

        currentState = NextState;
      } else {
        return false;
      }

      break;
    case State::Flush:
      if (  State::Lock == currentState
        ||  State::LockAll == currentState
        ||  State::LockAndAll == currentState
        ||  State::RemoteCommunicationLock == currentState
        ||  State::RemoteCommunicationLockAll == currentState
      ) {
        currentState = currentState; // Ok, check permission, but don't change state.
      } else {
        return false;
      }

      break;
    case State::Freed:
      if (  State::Complete == currentState
        ||  State::Created == currentState
        ||  State::Fence == currentState
        ||  State::NoPreceedFence == currentState
        ||  State::NoPreceedSucceedFence == currentState
        ||  State::NoSucceedFence == currentState
        ||  State::TestWait == currentState
      ) {
        currentState = NextState;
      } else {
        return false;
      }

      break;
    case State::Lock:
      if (  State::Complete == currentState
        ||  State::Created == currentState
        ||  (State::Fence == currentState && hadSecondFence)
        ||  State::Lock == currentState
        ||  State::LockAll == currentState
        ||  State::LockAndAll == currentState
        ||  (State::NoPreceedFence == currentState && hadSecondFence)
        ||  (State::NoPreceedSucceedFence == currentState && hadSecondFence)
        ||  (State::NoSucceedFence == currentState && hadSecondFence)
        ||  State::RemoteCommunicationLock == currentState
        ||  State::TestWait == currentState
      ) {
        lockCount += 1;

        if (State::LockAll == currentState) {
          currentState = State::LockAndAll;
        } else {
          currentState = NextState;
        }
      } else {
        return false;
      }

      break;
    case State::LockAll:
      if (  State::Complete == currentState
        ||  State::Created == currentState
        ||  (State::Fence == currentState && hadSecondFence)
        ||  (State::NoPreceedFence == currentState && hadSecondFence)
        ||  (State::NoPreceedSucceedFence == currentState && hadSecondFence)
        ||  (State::NoSucceedFence == currentState && hadSecondFence)
        ||  State::RemoteCommunicationLock == currentState
        ||  State::TestWait == currentState
      ) {
        currentState = NextState;
      } else if (State::Lock == currentState) {
        currentState = State::LockAndAll;
      } else {
        return false;
      }

      break;
    case State::NoPreceedFence:
      if (  State::Complete == currentState
        ||  State::Created == currentState
        ||  State::Fence == currentState
        ||  State::NoPreceedFence == currentState
        ||  State::NoPreceedSucceedFence == currentState
        ||  State::NoSucceedFence == currentState
        ||  State::TestWait == currentState
      ) {
        if (  State::Fence == currentState
          ||  State::NoPreceedFence == currentState
          ||  State::NoPreceedSucceedFence == currentState
          ||  State::NoSucceedFence == currentState
        ) {
          hadSecondFence = true;
        }

        currentState = NextState;
      } else {
        return false;
      }

      break;
    case State::NoPreceedSucceedFence:
      if (  State::Complete == currentState
        ||  State::Created == currentState
        ||  State::Fence == currentState
        ||  State::NoPreceedFence == currentState
        ||  State::NoPreceedSucceedFence == currentState
        ||  State::NoSucceedFence == currentState
        ||  State::TestWait == currentState
      ) {
        if (  State::Fence == currentState
          ||  State::NoPreceedFence == currentState
          ||  State::NoPreceedSucceedFence == currentState
          ||  State::NoSucceedFence == currentState
        ) {
          hadSecondFence = true;
        }

        currentState = NextState;
      } else {
        return false;
      }

      break;
    case State::NoSucceedFence:
      if (  State::Complete == currentState
        ||  State::Fence == currentState
        ||  State::NoPreceedFence == currentState
        ||  State::NoPreceedSucceedFence == currentState
        ||  State::NoSucceedFence == currentState
        ||  State::RemoteCommunicationFence == currentState
        ||  State::TestWait == currentState
      ) {
        if (  State::Fence == currentState
          ||  State::NoPreceedFence == currentState
          ||  State::NoPreceedSucceedFence == currentState
          ||  State::NoSucceedFence == currentState
        ) {
          hadSecondFence = true;
        }

        currentState = NextState;
      } else {
        return false;
      }

      break;
    case State::Post:
      if (  State::Complete == currentState
        ||  State::Created == currentState
        ||  (State::Fence == currentState && hadSecondFence)
        ||  (State::NoPreceedFence == currentState && hadSecondFence)
        ||  (State::NoPreceedSucceedFence == currentState && hadSecondFence)
        ||  (State::NoSucceedFence == currentState && hadSecondFence)
        ||  State::TestWait == currentState
      ) {
        currentState = NextState;
      } else if (State::Start == currentState) {
        currentState = State::PostStart;
      } else {
        return false;
      }

      break;
    case State::RemoteCommunication:
      if (  State::Fence == currentState
        ||  State::NoPreceedFence == currentState
        ||  State::RemoteCommunicationFence == currentState
      ) {
        currentState = State::RemoteCommunicationFence;
      } else if (
            State::PostStart == currentState
        ||  State::RemoteCommunicationGATS_PS == currentState
      ) {
        currentState = State::RemoteCommunicationGATS_PS;
      } else if (
            State::Start == currentState
        ||  State::RemoteCommunicationGATS == currentState
      ) {
        currentState = State::RemoteCommunicationGATS;
      } else if (
            State::Lock == currentState
        ||  State::RemoteCommunicationLock == currentState
      ) {
        currentState = State::RemoteCommunicationLock;
      } else if (
            State::LockAll == currentState
        ||  State::RemoteCommunicationLockAll == currentState
      ) {
        currentState = State::RemoteCommunicationLockAll;
      } else if (
            State::LockAndAll == currentState
        ||  State::RemoteCommunicationLockAndAll == currentState
      ) {
        currentState = State::RemoteCommunicationLockAndAll;
      } else {
        return false;
      }

      break;
    case State::RequestRemoteCommunication:
      if ( State::Lock == currentState
        || State::RemoteCommunicationLock == currentState
      ) {
        currentState = State::RemoteCommunicationLock;
      } else if (
            State::LockAll == currentState
        ||  State::RemoteCommunicationLockAll == currentState
      ) {
        currentState = State::RemoteCommunicationLockAll;
      } else if (
            State::LockAndAll == currentState
        ||  State::RemoteCommunicationLockAndAll == currentState
      ) {
        currentState = State::RemoteCommunicationLockAndAll;
      } else {
        return false;
      }

      break;
    case State::Start:
      if (  State::Complete == currentState
        ||  State::Created == currentState
        ||  (State::Fence == currentState && hadSecondFence)
        ||  (State::NoPreceedFence == currentState && hadSecondFence)
        ||  (State::NoPreceedSucceedFence == currentState && hadSecondFence)
        ||  (State::NoSucceedFence == currentState && hadSecondFence)
        ||  State::TestWait == currentState
      ) {
        currentState = NextState;
      } else if (State::Post == currentState) {
        currentState = State::PostStart;
      } else {
        return false;
      }

      break;
    case State::Sync:
      if (  State::Lock == currentState
        ||  State::LockAll == currentState
        ||  State::LockAndAll == currentState
        ||  State::RemoteCommunicationLock == currentState
        ||  State::RemoteCommunicationLockAll == currentState
      ) {
        currentState = currentState; // See Flush.
      } else {
        return false;
      }

      break;
    case State::TestWait:
      if (State::Post == currentState) {
        currentState = NextState;
      } else if (
            State::PostStart == currentState
        ||  State::RemoteCommunicationGATS_PS == currentState
      ) {
        currentState = State::Start;
      } else {
        return false;
      }

      break;
    case State::Unlock:
      if (  State::Lock == currentState
        ||  State::LockAndAll == currentState
        ||  State::RemoteCommunicationLock == currentState
        ||  State::RemoteCommunicationLockAndAll == currentState
      ) {
        lockCount -= 1;

        if (lockCount < 0) {
          return false;
        } else if (0 == lockCount) {
          if (State::LockAndAll == currentState) {
            currentState = State::LockAll;
          } else if (State::RemoteCommunicationLockAndAll == currentState) {
            currentState = State::RemoteCommunicationLockAll;
          } else {
            // Unsure if something remembers a call to `MPI_Win_fence` whatever
            // time ago. Not much different to Complete/TestWait.
            currentState = State::Created;
          }
        } // no change to SM if counter still above 0!
      } else {
        return false;
      }

      break;
    case State::UnlockAll:
      // TODO: Is this returning to any previous lock setup? Or unwinds it everything,
      // i.e. can we set `lockCount` to 0 then?
      if (  State::LockAll == currentState
        ||  State::LockAndAll == currentState
        ||  State::RemoteCommunicationLockAll == currentState
        ||  State::RemoteCommunicationLockAndAll == currentState
      ) {
          if (State::LockAndAll == currentState) {
            currentState = State::Lock;
          } else if (State::RemoteCommunicationLockAndAll == currentState) {
            currentState = State::RemoteCommunicationLock;
          } else {
            currentState = State::Created;
          }
      } else {
        return false;
      }

      break;
    default:
      return false;
  }

  return true;
}
