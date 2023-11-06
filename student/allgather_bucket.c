#include <assert.h>
#include <mpi.h>
#include <string.h>
#include <stdio.h>

int GT_Allgather( void *sendbuf, 
                  int  sendcount,
                  MPI_Datatype sendtype, 
                  void *recvbuf, 
                  int recvcount,
                  MPI_Datatype recvtype, 
                  MPI_Comm comm)
{
	assert(sendtype == MPI_INT && recvtype == MPI_INT);
    assert(sendcount == recvcount);
	
	/* Your code here (Do not just call MPI_Allgather) */
    int rank, size, prev, next;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);
 
    prev = rank - 1;
    if (prev < 0)
    {
        prev = size - 1;
    }
    next = rank + 1;
    if (next == size)
    {
        next = 0;
    }

    memcpy((int*)recvbuf + (rank)*recvcount, sendbuf, sendcount*sizeof(int));

    int pos = rank;
    
    for (int i = 0; i < size - 1; i++)
    {
        MPI_Request sendRequest, recvRequest;
        MPI_Isend((int*)recvbuf + (pos)*recvcount, recvcount, sendtype, next, 0, comm, &sendRequest);
        pos = pos - 1;
        if (pos < 0)
        {
            pos = size - 1;
        }
        MPI_Irecv((int*)recvbuf + (pos)*recvcount, recvcount, sendtype, prev, 0, comm, &recvRequest);
        MPI_Wait(&sendRequest, MPI_STATUS_IGNORE);
        MPI_Wait(&recvRequest, MPI_STATUS_IGNORE);   
    }
   
	return MPI_SUCCESS;
}
