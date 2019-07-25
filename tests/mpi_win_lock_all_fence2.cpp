#include "mpi.h"

/**
 * This example is probably legal; at least it seems to work fine.
 */

int main (int argc, char **argv) {
  MPI_Init(&argc, &argv);

  uint8_t memory = 0xEE, someValue = 0xFF;
  int rank = -1;
  MPI_Win window;

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  MPI_Win_create(
      &memory
    , sizeof(memory)
    , 1
    , MPI_INFO_NULL
    , MPI_COMM_WORLD
    , &window
  );

  if (0 == rank) {
    MPI_Win_fence(0, window);
    MPI_Win_fence(0, window);

    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);

    int res = MPI_Get(
        &someValue
      , 1
      , MPI_BYTE
      , 1
      , 0
      , 1
      , MPI_BYTE
      , window
    );

    MPI_Win_fence(0, window);
  } else {
    MPI_Win_fence(0, window);
    MPI_Win_fence(0, window);

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Win_lock_all(0, window);
    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Win_unlock_all(window);

    MPI_Win_fence(0, window);
  }

  MPI_Win_free(&window);

  MPI_Finalize();

  return 0;
}
