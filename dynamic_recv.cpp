#include <mpi.h>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cassert>
#include <vector>

#define RANK 2

/* Generate random numbers within a range */
int random_num(int high, int low)
{
  return ( (rand() % (high + 1)) );
}

int main(int argc, char** argv) 
{    
  int mpisize;
  int mpirank;
  int recv_count;
  MPI_Request req[RANK];
  MPI_Status status[RANK];

  /* MPI init */
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &mpisize);
  MPI_Comm_rank(MPI_COMM_WORLD, &mpirank);

  /* Lets keep it simple :) */
  assert(mpisize == 2);

  /* allocate buffer size */ 
  std::vector< std::vector<int> > send_bufs(mpisize);
  srand(time(0));
  int msg_size = random_num(10,1);
  send_bufs[0].resize(msg_size,4);
  //std::cout << send_bufs[0].size() << std::endl;
  msg_size = random_num(10,1);
  send_bufs[1].resize(msg_size,8);
  //std::cout << send_bufs[1].size() << std::endl;


  /* message receivers */
  std::vector< std::vector<int> > recv_bufs(mpisize);

  /* send messages */
  if(mpirank == 0)
  {
    std::cout << mpirank << " sent " << send_bufs[0].size() << " ints to rank 1" << std::endl;
    MPI_Isend( send_bufs[0].data(), send_bufs[0].size(), MPI_INT, 1, 0, MPI_COMM_WORLD, &req[0] );
  }

  if(mpirank == 1)
  {
    std::cout << mpirank << " sent " << send_bufs[1].size() << " ints to rank 0" << std::endl;
    MPI_Isend( send_bufs[1].data(), send_bufs[1].size(), MPI_INT, 0, 1, MPI_COMM_WORLD, &req[1] );
  }

  /* receive messages*/
  if(mpirank == 0)
  {
    MPI_Probe(1, 1, MPI_COMM_WORLD, &status[0]);

    MPI_Get_count( &status[0], MPI_INT, &recv_count);

    recv_bufs[0].resize(recv_count);

    MPI_Recv( recv_bufs[0].data(), recv_count, MPI_INT, 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE );
    std::cout << mpirank << " recvd " << recv_count << " ints from rank 1" << std::endl;
  }

  if(mpirank == 1)
  {
    MPI_Probe(0, 0, MPI_COMM_WORLD, &status[1]);

    MPI_Get_count( &status[1], MPI_INT, &recv_count);

    recv_bufs[1].resize(recv_count);

    MPI_Recv( recv_bufs[1].data(), recv_count, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE );
    std::cout << mpirank << " recvd " << recv_count << " ints from rank 0" << std::endl;
  }

  /* MPI close */
  MPI_Finalize();
  
  return EXIT_SUCCESS;
}
