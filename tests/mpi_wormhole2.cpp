#include "mpi.h"


int main (int argc, char **argv) {
  MPI_Init(&argc, &argv);

  size_t memory = 0;
  MPI_Comm other_comm;
  int rank = -1;

  int a = 0, b = 1, c = 2, x = 1;

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  MPI_Comm_split(
      MPI_COMM_WORLD
    , rank
    , rank
    , &other_comm
  );

  MPI_Win window;

  MPI_Win_create(
      &memory
    , sizeof(memory)
    , 1
    , MPI_INFO_NULL
    , other_comm
    , &window
  );

  MPI_Win_fence(0, window);

  if (rank > 0) {
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
  } else {
    MPI_Put(
        &rank
      , 1
      , MPI_INT
      , 0
      , rank
      , 1
      , MPI_INT
      , window
    );
  }

  if (x) {
    if (a && b && c) {
      MPI_Put(
          &rank
        , 1
        , MPI_INT
        , 0
        , rank
        , 1
        , MPI_INT
        , window
      );
    }
  }

  if (a || b || c) {
    MPI_Put(
        &rank
      , 1
      , MPI_INT
      , 0
      , rank
      , 1
      , MPI_INT
      , window
    );
  }

  MPI_Win_fence(0, window);

  MPI_Win_free(&window);

  MPI_Comm_free(&other_comm);

  MPI_Finalize();

  return 0;
}
