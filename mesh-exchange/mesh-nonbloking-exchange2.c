#include <stdio.h>
#include "mpi.h"

#define DATA_NUM 100000
#define LOOP_NUM 100

int main()
{
    int rank, size;
    int i, j;
    int left, right, up, down;
    MPI_Request req[8];
    MPI_Status status[8];
    int x[DATA_NUM];
    int y[DATA_NUM];

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != 12) {
        /* If the program has been run with < or > 12 processes,
         * abort */
        fprintf(stderr, "Run this program with 12 processes.\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    /* left neighbor is rank - 1 unless I am the first process in my
     * row */
    if (rank % 3)
        left = rank - 1;
    else
        left = -1;

    /* right neighbor is rank + 1 unless I am the last process in my
     * row */
    if ((rank + 1) % 3)
        right = rank + 1;
    else
        right = -1;

    /* up neighbor is rank + 4 unless I am in the top row */
    if (rank < 9)
        up = rank + 3;
    else
        up = -1;

    /* down neighbor is rank - 4 unless I am in the bottom row */
    if (rank >= 3)
        down = rank - 3;
    else
        down = -1;
    double start_time, end_time;
    start_time = MPI_Wtime();
    for (j = 0; j < LOOP_NUM; j++) {

      i = 0;

      MPI_Barrier(MPI_COMM_WORLD);

      /* Post receive messages */
      if (left != -1) {
          /* If left neighbor exists, recv a message */
          MPI_Irecv(y, DATA_NUM, MPI_INT, left, 0, MPI_COMM_WORLD, &req[i++]);
      }

      if (right != -1) {
          /* If right neighbor exists, recv a message */
          MPI_Irecv(y, DATA_NUM, MPI_INT, right, 0, MPI_COMM_WORLD, &req[i++]);
      }

      if (up != -1) {
          /* If up neighbor exists, recv a message */
          MPI_Irecv(y, DATA_NUM, MPI_INT, up, 0, MPI_COMM_WORLD, &req[i++]);
      }

      if (down != -1) {
          /* If down neighbor exists, recv a message */
          MPI_Irecv(y, DATA_NUM, MPI_INT, down, 0, MPI_COMM_WORLD, &req[i++]);
      }

      /* Post send messages */
      if (left != -1) {
          /* If left neighbor exists, send a message */
          MPI_Isend(x, DATA_NUM, MPI_INT, left, 0, MPI_COMM_WORLD, &req[i++]);
      }

      if (right != -1) {
          /* If right neighbor exists, send a message */
          MPI_Isend(x, DATA_NUM, MPI_INT, right, 0, MPI_COMM_WORLD, &req[i++]);
      }

      if (up != -1) {
          /* If up neighbor exists, send a message */
          MPI_Isend(x, DATA_NUM, MPI_INT, up, 0, MPI_COMM_WORLD, &req[i++]);
      }

      if (down != -1) {
          /* If down neighbor exists, send a message */
          MPI_Isend(x, DATA_NUM, MPI_INT, down, 0, MPI_COMM_WORLD, &req[i++]);
      }

      /* Wait for everything to finish */
      MPI_Waitall(i, req, MPI_STATUSES_IGNORE);
    }
    end_time = MPI_Wtime();
    printf("run for %d, total time is %f seconds, average time is %f \n",LOOP_NUM,end_time-start_time,end_time-start_time/LOOP_NUM);

    MPI_Finalize();

    return 0;
}
