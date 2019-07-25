#include <iostream>

#include "mpi.h"

int main (int argc, char **argv) {
  MPI_Init(&argc, &argv);

  size_t memory = 0;
  int rank = -1;

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (0 == rank) {
    MPI_Win window;

    MPI_Win_create(
        &memory
      , sizeof(memory)
      , 1
      , MPI_INFO_NULL
      , MPI_COMM_WORLD
      , &window
    );

    MPI_Win_free(&window);
  } else {
    MPI_Win window;
    void *address = nullptr;

    MPI_Win_allocate_shared(
        sizeof(memory)
      , 1
      , MPI_INFO_NULL
      , MPI_COMM_WORLD
      , &address
      , &window
    );

    MPI_Win_free(&window);
  }

  MPI_Finalize();

  return 0;
}
