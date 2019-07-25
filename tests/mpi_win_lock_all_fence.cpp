#include "mpi.h"

/**
 * This example is legal!
 */

int main (int argc, char **argv) {
  MPI_Init(&argc, &argv);

  uint8_t memory[128] = {0};
  int rank = -1, someValue = 0;
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
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Win_fence(0, window);
    MPI_Win_fence(0, window);
  } else {
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Win_lock_all(0, window);

    MPI_Get(
        &someValue
      , 1
      , MPI_INT
      , 0
      , 0
      , 1
      , MPI_INT
      , window
    );

    MPI_Win_unlock_all(window);

    MPI_Win_fence(0, window);
    MPI_Win_fence(0, window);
  }

  MPI_Win_free(&window);

  MPI_Finalize();

  return 0;
}
