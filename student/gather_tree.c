#include <assert.h>
#include <mpi.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void MSTGather(void *sendbuf, void**recvbufptr, int count, MPI_Datatype datatype, int root, int rank, MPI_Comm comm, int left, int right){
	

	int mid = floor((left + right) / 2);
	int src = (root <= mid) ? right : left;

	if (rank <= mid && root <= mid)
	{
		if (left != mid)
		{
			MSTGather(sendbuf, recvbufptr, count, datatype, root, rank, comm, left, mid);
		}
		
		
	} else if (rank <= mid && root > mid)
	{
		if (left != mid)
		{
			MSTGather(sendbuf, recvbufptr, count, datatype, src, rank, comm, left, mid);
		}
		
	} else if (rank > mid && root <= mid)
	{
		if (mid + 1 != right)
		{
			MSTGather(sendbuf, recvbufptr, count, datatype, src, rank, comm, mid + 1, right);
		}
		
	} else if (rank > mid && root > mid)
	{
		if (mid + 1 != right)
		{
			MSTGather(sendbuf, recvbufptr, count, datatype, root, rank, comm, mid + 1, right);
		}
		
	}
	

	if (root <= mid)
	{
		int size = (right - mid)*count;
		if (rank == src)
		{
			if (size == count)
			{
				MPI_Send(sendbuf, size, datatype, root, 0, comm);
				return;
			} 
			int* buffer_start = (int*)(*recvbufptr) + (mid + 1)*count;
			MPI_Send(buffer_start, size, datatype, root, 0, comm);
			
			
		} else if (rank == root)
		{
			int total_size;
			MPI_Comm_size(comm, &total_size);
			if (*recvbufptr == NULL)
			{
				int *recvbuf = malloc(total_size * count * sizeof(int));
				memcpy((int*)recvbuf + (left)*count, sendbuf, count*sizeof(int));
				*recvbufptr = recvbuf;
			}
			int* buffer_start = (int*)(*recvbufptr) + (mid + 1)*count;
			MPI_Recv(buffer_start, size, datatype, src, 0, comm, MPI_STATUS_IGNORE);
		}
		
	} else
	{
		int size = (mid - left + 1)*count;
		if (rank == src)
		{
			if (size == count)
			{
				MPI_Send(sendbuf, size, datatype, root, 0, comm);
				return;
			}
			int* buffer_start = (int*)(*recvbufptr) + (left)*count;
			MPI_Send(buffer_start, size, datatype, root, 0, comm);
		} else if (rank == root)
		{
			int total_size;
			MPI_Comm_size(comm, &total_size);
			if (*recvbufptr == NULL)
			{
				
				int* recvbuf = malloc(total_size * count * sizeof(int));
				memcpy((int*)recvbuf + (mid+1)*count, sendbuf, count*sizeof(int));
				*recvbufptr = recvbuf;

			}
			int* buffer_start = (int*)(*recvbufptr) + (left)*count;
			MPI_Recv(buffer_start, size, datatype, src, 0, comm, MPI_STATUS_IGNORE);
		}
	}
	
}

int GT_Gather(void *sendbuf, int sendcount, MPI_Datatype sendtype,
		void *recvbuf, int recvcount, MPI_Datatype recvtype, int root,
		MPI_Comm comm)
{
	assert(sendtype == MPI_INT && recvtype == MPI_INT);
	assert(root == 0);
	
	/* Your code here (Do not just call MPI_Gather) */
	int rank, size;
   	MPI_Comm_rank(comm, &rank);
   	MPI_Comm_size(comm, &size);

   	int left = 0;
   	int right = size - 1;

	if (recvbuf != NULL)
	{
		memcpy((int*)recvbuf, sendbuf, sendcount*sizeof(int));
	} /* else
	{
		recvbuf = malloc(size * sendcount * sizeof(int));
		memcpy((int*)recvbuf + rank*sendcount, sendbuf, sendcount*sizeof(int));
	} */
	

	MSTGather(sendbuf, &recvbuf, sendcount, sendtype, root, rank, comm, left, right);
	
	
	if (rank != 0)
	{
		if	(recvbuf != NULL)
		{
			free(recvbuf);
		}
	}
	
	
	return MPI_SUCCESS;
}
