#include "mpi.h"

bool whatIsTruth () {
  return true;
}

int main (int argc, char **argv) {
  MPI_Init(&argc, &argv);

  bool someTruth = whatIsTruth();
  uint8_t memory[128] = {0};
  int rank = -1, someValue = 0;
  MPI_Win window;
  MPI_Group group;

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_group(MPI_COMM_WORLD, &group);

  MPI_Win_create(
      &memory
    , sizeof(memory)
    , 1
    , MPI_INFO_NULL
    , MPI_COMM_WORLD
    , &window
  );

  if (0 == rank) {
    if (someTruth) {
      MPI_Win_start(group, 0, window);

      MPI_Get(
          &someValue
        , 1
        , MPI_INT
        , 0
        , rank
        , 1
        , MPI_INT
        , window
      );

      MPI_Win_complete(window);
    } else {
      // also rank 0!
      MPI_Win_post(group, 0, window);
      MPI_Win_wait(window);
    }
  } else {
    // rank != 0
    memory[0] = 123;
  }

  MPI_Win_free(&window);

  MPI_Finalize();

  return 0;
}
