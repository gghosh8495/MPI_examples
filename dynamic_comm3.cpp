/*
Script to demonstrate one sided MPI comm where one rank
recieves buffers from multiple peer ranks and the size
of the incoming buffers is unknown.

Author - Gautam Ghosh
Date - 04/23/2022
*/

#include <cstdlib>
#include <vector>
#include <cassert>
#include <algorithm>
#include <mpi.h>

#define RECEIVER 0

int main(int argc, char **argv)
{
  int mpisize, mpirank, mpiret, mpitag;
  int msg_count, sender;
  std::vector<int> sb1, sb2; /* Send buffers */
  std::vector<int> rb;       /* Recv buffer  */
  MPI_Request sreq;
  MPI_Request rreq[2];
  MPI_Status stat[2];

  /* MPI init */
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &mpisize);
  MPI_Comm_rank(MPI_COMM_WORLD, &mpirank);

  assert(mpisize == 3);
  mpitag = 101;

  if (mpirank == 1)
  {
    /* allocate buffer to be sent to rank 0 */
    sb1.resize(4);
    std::fill(sb1.begin(), sb1.end(), 4);

    /* send buffer */
    mpiret = MPI_Isend(sb1.data(), (int)sb1.size(), MPI_INT, RECEIVER, mpitag, MPI_COMM_WORLD, &sreq);
    assert(mpiret == MPI_SUCCESS);
    printf("\nrank 1 sends 4 ints of total size %ld bytes to rank 0\n", 4 * sizeof(int));
  }

  if (mpirank == 2)
  {
    /* allocate buffer to be sent to rank 0 */
    sb2.resize(8);
    std::fill(sb2.begin(), sb2.end(), 8);

    /* send buffer */
    mpiret = MPI_Isend(sb2.data(), (int)sb2.size(), MPI_INT, RECEIVER, mpitag, MPI_COMM_WORLD, &sreq);
    assert(mpiret == MPI_SUCCESS);
    printf("\nrank 2 sends 8 ints of total size %ld bytes to rank 0\n", 8 * sizeof(int));
  }

  /* Wait for messages to be sent out */
  if (mpirank != 0)
  {
    mpiret = MPI_Wait(&sreq, MPI_STATUS_IGNORE);
    assert(mpiret == MPI_SUCCESS);

    assert(sreq == MPI_REQUEST_NULL);
  }

  if (mpirank == 0)
  {
    for (int peer_rank = 1; peer_rank < mpisize; ++peer_rank)
    {
      /* probe to obtain MPI_Status object */
      mpiret = MPI_Probe(peer_rank, mpitag, MPI_COMM_WORLD, &stat[peer_rank - 1]);
      assert(mpiret == MPI_SUCCESS);

      /* extract info out of MPI_Status object */
      int counter = 0;
      mpiret = MPI_Get_count(&stat[peer_rank - 1], MPI_INT, &msg_count);
      assert(mpiret == MPI_SUCCESS);
      sender = stat[peer_rank - 1].MPI_SOURCE;
      assert(sender != 0);

      /* allocate space to recieve data */
      rb.resize(msg_count);

      /* receive incoming buffer */
      mpiret = MPI_Irecv(rb.data(), msg_count,
                         MPI_INT, sender, mpitag, MPI_COMM_WORLD,
                         &rreq[peer_rank - 1]);
      assert(mpiret == MPI_SUCCESS);
      printf("\nrank 0 recvs %d ints of total size %ld bytes from rank %d\n", (int)rb.size(), rb.size() * sizeof(int), sender);
    }
    /* make sure all messages have been received */
    mpiret = MPI_Waitall(2, rreq, MPI_STATUSES_IGNORE);
    assert(mpiret == MPI_SUCCESS);
  }

  /* MPI exit */
  MPI_Finalize();

  return EXIT_SUCCESS;
}
