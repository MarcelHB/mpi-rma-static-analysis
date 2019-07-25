#include "mpi.h"

struct Context {
  size_t value;
  MPI_Win window;
};

int main (int argc, char **argv) {
  MPI_Init(&argc, &argv);

  size_t memory = 0;
  MPI_Comm other_comm;
  int rank = -1;

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  MPI_Comm_split(
      MPI_COMM_WORLD
    , rank
    , rank
    , &other_comm
  );

  Context ctx;
  ctx.value = 1;
  Context *ctx2 = &ctx;

  MPI_Win_create(
      &memory
    , sizeof(memory)
    , 1
    , MPI_INFO_NULL
    , other_comm
    , &(ctx2->window)
  );

  MPI_Win_fence(0, ctx.window);

  MPI_Get(
      &rank
    , 1
    , MPI_INT
    , 0
    , rank
    , 1
    , MPI_INT
    , ctx.window
  );

  MPI_Put(
      &rank
    , 1
    , MPI_INT
    , 0
    , rank
    , 1
    , MPI_INT
    , ctx2->window
  );

  MPI_Win_fence(0, ctx.window);

  MPI_Win_free(&(ctx.window));

  MPI_Comm_free(&other_comm);

  MPI_Finalize();

  return 0;
}
