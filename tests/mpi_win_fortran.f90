PROGRAM main
  USE mpi

  INTEGER :: error = 0
  INTEGER :: rank = -1
  INTEGER :: window_memory = 0
  INTEGER :: local_value = 0
  INTEGER :: window = 0

  CALL mpi_init(error)
  CALL mpi_comm_rank(MPI_COMM_WORLD, rank, error)

  CALL mpi_win_create(window_memory, SIZEOF(window_memory), SIZEOF(window_memory), MPI_INFO_NULL, MPI_COMM_WORLD, window, error)

  CALL mpi_barrier(MPI_COMM_WORLD, error)

  CALL mpi_win_fence(0, window, error)

  IF(rank .EQ. 0) THEN
    CALL mpi_get(local_value, 1, MPI_INTEGER, 1, 0, 1, MPI_INTEGER, window, error)
    local_value = local_value + 1
  ELSE
    DO i=1,10
      DO j=i,i+10
        CALL mpi_put(local_value, 1, MPI_INTEGER, 0, 0, 1, MPI_INTEGER, window, error)
      END DO
    END DO
  END IF

  CALL mpi_win_fence(0, window, error)

  CALL mpi_win_free(window, error)

  CALL mpi_finalize(error)
END PROGRAM main
