#include <cstdlib>
#include <cassert>
#include <vector>
#include <mpi.h>

/*
Script to demonstrate two way MPI comm when the size of the incoming buffer
size is unknown.

Author - Gautam Ghosh
Date - 04/23/2022
*/

int main(int argc, char **argv)
{
	int mpirank, mpisize, ii, msg_count, mpiret, sender;
	int *sb0, *sb1;			// send buffers
	int *rb0, *rb1;			// recv buffers
	MPI_Request req[1]; // Request handle to be used by MPI_Isend
	MPI_Status stat[2]; // Status handle to be used by MPI_Probe, MPI_Get_count

	/* MPI init */
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &mpirank);
	MPI_Comm_size(MPI_COMM_WORLD, &mpisize);

	/* This example is used to demonstrate two way comm */
	assert(mpisize == 2);
	std::cout << "\ncurrent MPI rank =  " << mpirank << std::endl;

	/* allocate buffers which need to be communicated */
	const size_t count0 = 4;
	const size_t count1 = 8;
	sb0 = (int *)calloc(count0, sizeof(int)); /* this buffer will go to rank 1 */
	sb1 = (int *)calloc(count1, sizeof(int)); /* this buffer will go to rank 0 */
	sb0[0] = sb0[1] = sb0[2] = sb0[3] = 4;
	sb1[0] = sb1[1] = sb1[2] = sb1[3] = 8;
	rb0 = nullptr;
	rb1 = nullptr;

	/* send messages with MPI Isend */
	if (mpirank == 0)
	{
		mpiret = MPI_Isend(sb0, (int)count0, MPI_INT, 1, 0, MPI_COMM_WORLD, &req[0]);
		assert(mpiret == MPI_SUCCESS);
		printf("\nmpirank %d sends %d ints of total size %ld bytes to rank %d\n", mpirank, (int)count0, count0 * sizeof(int), 1);
	}

	/* send messages with MPI Isend */
	if (mpirank == 1)
	{
		mpiret = MPI_Isend(sb1, (int)count1, MPI_INT, 0, 1, MPI_COMM_WORLD, &req[0]);
		assert(mpiret == MPI_SUCCESS);
		printf("\nmpirank %d sends %d ints of total size %ld bytes to rank %d\n", mpirank, (int)count1, count1 * sizeof(int), 0);
	}

	mpiret = MPI_Waitall(1, req, MPI_STATUSES_IGNORE);
	assert(mpiret == MPI_SUCCESS);

	/* receive buffer */
	if (mpirank == 0)
	{
		/* probe to obtain MPI_Status object */
		mpiret = MPI_Probe(1, 1, MPI_COMM_WORLD, &stat[0]);
		assert(mpiret == MPI_SUCCESS);

		/* extract info out of the MPI_Status object */
		mpiret = MPI_Get_count(&stat[0], MPI_INT, &msg_count);
		assert(mpiret == MPI_SUCCESS);
		sender = stat[0].MPI_SOURCE;
		assert(sender != mpirank);

		/* allocate memory for recv buffer */
		rb0 = (int *)malloc(msg_count * sizeof(int));
		printf("\nint count = %d\n", msg_count);

		/* recieve the incoming buffer */
		mpiret = MPI_Recv(rb0, msg_count, MPI_INT, sender, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		assert(mpiret == MPI_SUCCESS);
		printf("\nmpirank %d recvs %d ints of total size %ld bytes from rank %d\n", mpirank, msg_count, msg_count * sizeof(int), sender);

		std::cout << "\ncontents of the buffer from rank 1 " << std::endl;
		for (ii = 0; ii < msg_count; ++ii)
			std::cout << rb0[ii] << std::endl;
	}

	/* receive buffer */
	if (mpirank == 1)
	{
		/* probe to obtain MPI_Status object */
		mpiret = MPI_Probe(0, 0, MPI_COMM_WORLD, &stat[1]);
		assert(mpiret == MPI_SUCCESS);

		/* extract info out of the MPI_Status object */
		mpiret = MPI_Get_count(&stat[1], MPI_INT, &msg_count);
		assert(mpiret == MPI_SUCCESS);
		sender = stat[1].MPI_SOURCE;
		assert(sender != mpirank);

		/* allocate memory for recv buffer */
		rb1 = (int *)malloc(msg_count * sizeof(int));
		printf("\nint count = %d\n", msg_count);

		/* recieve the incoming buffer */
		mpiret = MPI_Recv(rb1, msg_count, MPI_INT, sender, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		assert(mpiret == MPI_SUCCESS);
		printf("\nmpirank %d recvs %d ints of total size %ld bytes from rank %d\n", mpirank, msg_count, msg_count * sizeof(int), sender);

		std::cout << "\ncontents of the buffer from rank 0 " << std::endl;
		for (ii = 0; ii < msg_count; ++ii)
			std::cout << rb1[ii] << std::endl;
	}

	/* clean up memory */
	free(sb0);
	free(sb1);
	free(rb0);
	free(rb1);

	/* MPI exit */
	MPI_Finalize();

	return EXIT_SUCCESS;
}
