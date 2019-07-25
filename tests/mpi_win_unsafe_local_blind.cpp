#include "mpi.h"

uint8_t getIndex () {
  return 9;
}

int main (int argc, char **argv) {
  MPI_Init(&argc, &argv);

  uint8_t memory[128] = {0}, value = 0, localMemory[16] = {0};
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

  if (rank > 0) {
    MPI_Get(
        &localMemory
      , 1
      , MPI_INT
      , 0
      , rank
      , 1
      , MPI_INT
      , window
    );

    size_t i = getIndex();
    value = localMemory[i];
  }

  MPI_Win_fence(0, window);

  MPI_Win_free(&window);

  MPI_Finalize();

  return 0;
}
