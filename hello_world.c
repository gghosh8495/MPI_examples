
#include <stdio.h>

#include "mpi.h"

int main(int argc, char** argv)
{
    int rank;
    int comm_size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

    if(rank == 0)
    {
        printf("\nTotal procs = %d\n", comm_size);
    }
    printf("\nHello from proc %d\n", rank);

    MPI_Finalize();

    return 0;
}