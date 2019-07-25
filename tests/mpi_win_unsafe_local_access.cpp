#include "mpi.h"

int main (int argc, char **argv) {
  MPI_Init(&argc, &argv);

  uint8_t memory[128] = {0}, value = 0;
  int rank = -1;

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  MPI_Win window;

  MPI_Win_create(
      &memory
    , sizeof(memory)
    , 1
    , MPI_INFO_NULL
    , MPI_COMM_WORLD
    , &window
  );

  MPI_Win_fence(0, window);

  if (rank >= 1) {
    MPI_Get(
        &value
      , 1
      , MPI_INT
      , 0
      , rank
      , 1
      , MPI_INT
      , window
    );

    memory[0] = value + 1;
  } else {
    MPI_Put(
        &value
      , 1
      , MPI_INT
      , 0
      , rank
      , 1
      , MPI_INT
      , window
    );

    value = 2;
  }
  MPI_Win_fence(0, window);

  MPI_Win_free(&window);

  MPI_Finalize();

  return 0;
}
