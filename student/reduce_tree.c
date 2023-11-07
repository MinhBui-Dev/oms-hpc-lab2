#include <assert.h>
#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>


void MSTReduce(void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype,
			MPI_Op op, int root, MPI_Comm comm, int left, int right, int rank)
{
   int mid = floor((left + right) / 2);
   int src;
   if (root <= mid)
   {
      src = right;
   } else {
      src = left;
   }

   if (rank <= mid && root <= mid)
	{
      if (left != mid)
      {
         MSTReduce(sendbuf, recvbuf, count, datatype, op, root, comm, left, mid, rank);
      }
	} else if (rank <= mid && root > mid)
	{
      if (left != mid)
      {
         MSTReduce(sendbuf, recvbuf, count, datatype, op, src, comm, left, mid, rank);
      }
	} else if (rank > mid && root <= mid)
	{
      if (mid + 1 != right)
      {
         MSTReduce(sendbuf, recvbuf, count, datatype, op, src, comm, mid + 1, right, rank);
      }
	} else if (rank > mid && root > mid)
	{
      if (mid + 1 != right)
      {
         MSTReduce(sendbuf, recvbuf, count, datatype, op, root, comm, mid + 1, right, rank);
      }
	}

   if (rank == src)
   {
      MPI_Send(sendbuf, count, datatype, root, 0, comm);
   } else if (rank == root)
   {
      if (recvbuf == NULL)
      {
         recvbuf = malloc(count * sizeof(int));
      }
      MPI_Recv(recvbuf, count, datatype, src, 0, comm, MPI_STATUS_IGNORE);
      MPI_Reduce_local(recvbuf, sendbuf, count, datatype, op);
   }
}
int GT_Reduce(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype,
			MPI_Op op, int root, MPI_Comm comm)
{
   assert(datatype == MPI_INT);
   assert(op == MPI_SUM);
   assert(root == 0);

   // create intermediate buffer
   void *interbuf = malloc(count * sizeof(int));
   memcpy(interbuf, sendbuf, count * sizeof(int));
					
   /* Your code here (Do not just call MPI_Reduce) */
   int rank, size;
   MPI_Comm_rank(comm, &rank);
   MPI_Comm_size(comm, &size);

   int left = 0;
   int right = size - 1;

   MSTReduce(interbuf, recvbuf, count, datatype, op, root, comm, left, right, rank);
   if (rank == root)
   {
      memcpy(recvbuf, interbuf, count * sizeof(int));
   }
   
   free(interbuf);

   return MPI_SUCCESS;
}

