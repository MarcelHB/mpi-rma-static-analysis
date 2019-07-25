#include <iostream>

#include "mpi.h"

int main (int argc, char **argv) {
  std::cout << "=== Test ===" << std::endl;

  MPI_Init(&argc, &argv);

  size_t memory = 0, memory2 = 1;
  int rank = -1;

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (0 == rank) {
    MPI_Win window_A0;
    MPI_Win window_A1;

    MPI_Win_create(
        &memory
      , sizeof(memory)
      , 1
      , MPI_INFO_NULL
      , MPI_COMM_WORLD
      , &window_A0
    );

    MPI_Win_create(
        &memory2
      , sizeof(memory2)
      , 1
      , MPI_INFO_NULL
      , MPI_COMM_WORLD
      , &window_A1
    );

    std::cout << "* 0: A0 vs B0" << std::endl;
    MPI_Win_fence(0, window_A0);

    std::cout << "* 0: A1 vs B1" << std::endl;
    MPI_Win_fence(0, window_A1);

    std::cout << "* 0: A0 vs B1" << std::endl;
    MPI_Win_fence(0, window_A0);

    std::cout << "* 0: end" << std::endl;
    MPI_Win_free(&window_A0);
    MPI_Win_free(&window_A1);
  } else {
    MPI_Win window_B0;
    MPI_Win window_B1;

    MPI_Win_create(
        &memory
      , sizeof(memory)
      , 1
      , MPI_INFO_NULL
      , MPI_COMM_WORLD
      , &window_B0
    );

    MPI_Win_create(
        &memory2
      , sizeof(memory2)
      , 1
      , MPI_INFO_NULL
      , MPI_COMM_WORLD
      , &window_B1
    );

    std::cout << "* >0: B0 vs A0" << std::endl;
    MPI_Win_fence(0, window_B0);

    std::cout << "* >0: B1 vs A1" << std::endl;
    MPI_Win_fence(0, window_B1);

    std::cout << "* >0: B1 vs A0" << std::endl;
    MPI_Win_fence(0, window_B1);

    std::cout << "* >0: end" << std::endl;
    MPI_Win_free(&window_B0);
    MPI_Win_free(&window_B1);
  }

  MPI_Finalize();

  return 0;
}
