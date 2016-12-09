/*
 * Aaron Knister
 * Adapted from code written by Daniel Duffy
 */

#include "mpi.h"
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

/* Set the Debug output level */
int windowSize = 1;             /* Number of sends between nodes */

/* Declare the functions */
int MPI_Init(int *argc, char ***argv);
int MPI_Finalize();

/* Start the main program */
int
main(int argc, char *argv[])
{
  int numProcs;                 /* number of mpi processes */
  int myRank;                   /* local mpi rank */
  int i, j, trial;              /* loop variables */
  int x, y;
  int status;                   /* status of mpi calls */
  int rc;			/* store rc of various calls */
  int size = 830592;		/* Set the size of the messages */
  int outFileFd;

  char *outFileName;

  uint32_t sendBuffer[size];
  uint32_t recvBuffer[size];

  MPI_Request aSendRequest[windowSize];
  MPI_Request aRecvRequest[windowSize];


  /* Initialize the MPI processes */
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

  /* Set up the send and recieve buffers */
  for (i = 0; i < size; i++)
    {
      sendBuffer[i] = 86753.09;
      recvBuffer[i] = 12345678;
    }

  MPI_Barrier(MPI_COMM_WORLD);

  /* Begin MPI_ISend */
  printf("MPI_ISend from %d starting\n", myRank);
  for (x = 0; x < numProcs; x++)
    {
      for (j = 0; j < windowSize; j++)
        {
          MPI_Isend(&sendBuffer, size, MPI_REAL, x, 100, MPI_COMM_WORLD,
                    aSendRequest + j);
        }
    }
  printf("MPI_ISend from %d done\n", myRank);
  /* End MPI_ISend */


  /* Open output file */
  rc = asprintf(&outFileName, "pp_out.%d.dat", myRank);
  if (rc < 0)
    {
      printf("Failed to allocate string for output filename!\n");
      MPI_Abort(MPI_COMM_WORLD, 1);
    }
  outFileFd = open(outFileName, O_CREAT | O_RDWR, 0644);
  if (outFileFd < 0)
    {
      printf("Failed ot open output dat file! (%s)\n", strerror(errno));
      MPI_Abort(MPI_COMM_WORLD, 1);
    }
  lseek(outFileFd, 0, SEEK_END);

  /* Begin MPI_Recv */
  printf("MPI_Recv to %d starting\n", myRank);
  for (y = (numProcs - 1); y >= 0; y--)
    {
      for (j = 0; j < windowSize; j++)
        {
          MPI_Recv(&recvBuffer, size, MPI_REAL, y, 100, MPI_COMM_WORLD,
                   aRecvRequest + j);
          write(outFileFd, recvBuffer, size);
        }
    }
  printf("MPI_Recv to %d done\n", myRank);
  /* End MPI_Recv */

  /* Cleanup output file */
  close(outFileFd);
  free(outFileName);

  MPI_Finalize();

  return 0;
}
