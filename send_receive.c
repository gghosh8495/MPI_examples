
#include <stdio.h>
#include <string.h>

#include "mpi.h"

int main(int argc, char** argv)
{
    int my_rank;        /* rank of current process */
    int p;              /* number of processes */
    int source;         /* rank of sender */
    int dest;           /* rank of receiver */
    int tag = 0;        /* tag for messages */
    char msg[100];      /* message to be sent*/
    MPI_Status status;  /* return status for receive */

    /* MPI env init */
    MPI_Init(&argc, &argv);

    /* Get the rank of current process */
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    /* Get the total processes available */
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    /* Broadcast total processes available */
    printf("Total processes available = %d\n", p);

    /* Every process 'p' send a personal message to process 'p-1' */
    if(my_rank%2 != 0)
    {
        /* Prepare message */
        sprintf(msg, "Hello from process %d", my_rank);
        dest = my_rank -1;

        /* send message */
        MPI_Send(msg, strlen(msg)+1, MPI_CHAR, dest, tag, MPI_COMM_WORLD);
    }
    else
    {
        /* Receive message */
        source = my_rank + 1;
        MPI_Recv(msg, 100, MPI_CHAR, source, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        /* Read the message */
        printf(" process %d reads %s\n", my_rank, msg);
    }

    /* MPI env close */
    MPI_Finalize();

    return 0;
}