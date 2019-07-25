#include "mpi.h"

#define WINDOW_SIZE 128

int main (int argc, char **argv) {
  MPI_Init(&argc, &argv);

  int rank = -1;
  int masterRank = 0;
  MPI_Win window;
  uint8_t *local_shared_memory = nullptr;
  uint8_t *other_shared_memory = nullptr;

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  MPI_Win_allocate_shared(
      WINDOW_SIZE
    , 1
    , MPI_INFO_NULL
    , MPI_COMM_WORLD
    , &local_shared_memory
    , &window
  );

  if (0 == rank) {
    MPI_Win_fence(0, window);

    local_shared_memory[0] = 123;

    MPI_Win_fence(0, window);
  } else {
    MPI_Win_fence(0, window);
    MPI_Aint size = 0;
    int disp = 0;

    MPI_Win_shared_query(
        window
      , masterRank
      , &size
      , &disp
      , &other_shared_memory
    );

    other_shared_memory[0] = 17;

    MPI_Win_fence(0, window);
  }

  MPI_Win_free(&window);

  MPI_Finalize();

  return 0;
}
