#include "mpi.h"

int main (int argc, char **argv) {
  MPI_Init(&argc, &argv);

  size_t memory = 0;
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

  MPI_Get(
      &rank
    , 1
    , MPI_INT
    , 0
    , rank
    , 1
    , MPI_INT
    , window
  );

  MPI_Win_fence(0, window);

  if (rank > 0) {
    size_t alsoVeryLocalMemory = 0;

    MPI_Win_attach(window, &alsoVeryLocalMemory, sizeof(size_t));
  }

  MPI_Win_fence(0, window);

  MPI_Win_free(&window);

  MPI_Finalize();

  return 0;
}
