#include <assert.h>
#include <mpi.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

void MSTScatter(void *sendbuf, void*recvbuf, int count, MPI_Datatype datatype, int root, int rank, MPI_Comm comm, int left, int right, int offset){
	

	int mid = floor((left + right) / 2);
	int dest = (root <= mid) ? right : left;
	
	int left_pos = 0;
	int mid_pos = mid - left;
	// if root is in the left subtree, send buffer to right subtree
	// if root is in the right subtree, send buffer to left subtree
	if (root <= mid)
	{
		// buffer elements from mid + 1 to right
		int size = (right - mid)*count;
		
		if (rank == root)
		{
			//printf("root %d send to %d\n", root, dest);
			int* buffer_start = (int*)sendbuf + (mid_pos + 1)*count;
			MPI_Request request;
			MPI_Isend(buffer_start, size, datatype, dest, 0, comm, &request);
			if ((mid - left) == 0)
			{
				memcpy(recvbuf, (int*)sendbuf + (left_pos)*count, count*sizeof(int));
			}
			MPI_Wait(&request, MPI_STATUS_IGNORE);

		} else if (rank == dest)
		{
			// address of elements from mid + 1 to right
			if (size == count)
			{
				MPI_Recv(recvbuf, size, datatype, root, 0, comm, MPI_STATUS_IGNORE);
			} else
			{
				sendbuf = malloc(size * sizeof(int));
				MPI_Recv(sendbuf, size, datatype, root, 0, comm, MPI_STATUS_IGNORE);
			}
		}
		
	} else 
	{
		// send buffer elements from left to mid
		int size = (mid - left + 1)*count;
		
		if (rank == root)
		{
			//printf("root %d send to %d with offset %d\n", root, dest, offset);
			int* buffer_start = (int*)sendbuf + (left_pos + offset)*count;
			MPI_Request request;
			MPI_Isend(buffer_start, size, datatype, dest, 0, comm, &request);
			if ((right - mid) == 1)
			{
				memcpy(recvbuf, (int*)sendbuf + (mid_pos + 1 + offset)*count, count*sizeof(int));				
			} 
			offset += mid - left + 1;
			MPI_Wait(&request, MPI_STATUS_IGNORE);

		} else if (rank == dest)
		{
			// receive buffer elements from left to mid
			if (size == count)
			{
				MPI_Recv(recvbuf, size, datatype, root, 0, comm, MPI_STATUS_IGNORE);
			} else
			{
				sendbuf = malloc(size * sizeof(int));
				MPI_Recv(sendbuf, size, datatype, root, 0, comm, MPI_STATUS_IGNORE);	
			}
		}
	}

	if (rank <= mid && root <= mid)
	{
		if	(left == mid)
		{
			return;
		}
		MSTScatter(sendbuf, recvbuf, count, datatype, root, rank, comm, left, mid, offset);
	} else if (rank <= mid && root > mid)
	{
		if	(left == mid)
		{
			return;
		}
		MSTScatter(sendbuf, recvbuf, count, datatype, dest, rank, comm, left, mid, offset);
	} else if (rank > mid && root <= mid)
	{
		if	(mid + 1 == right)
		{
			return;
		}
		MSTScatter(sendbuf, recvbuf, count, datatype, dest, rank, comm, mid + 1, right, offset);
	} else if (rank > mid && root > mid)
	{
		if	(mid + 1 == right)
		{
			return;
		}
		MSTScatter(sendbuf, recvbuf, count, datatype, root, rank, comm, mid + 1, right, offset);
	}
	
	
}

int GT_Scatter(void *sendbuf, int sendcount, MPI_Datatype sendtype,
    void *recvbuf, int recvcount, MPI_Datatype recvtype, int root,
    MPI_Comm comm)
{
	assert(sendtype == MPI_INT && recvtype == MPI_INT);
	assert(root == 0);
	
	/* Your code here (Do not just call MPI_Scatter) */
	int rank, size;
	MPI_Comm_rank(comm, &rank);
	MPI_Comm_size(comm, &size);

	int left = 0;
	int right = size - 1;

	MSTScatter(sendbuf, recvbuf, sendcount, sendtype, root, rank, comm, left, right, 0);

	/* printf("rank: %d, recvbuf: ", rank);
	for (size_t i = 0; i < recvcount; i++)
	{
		printf("%d ",((int*)recvbuf)[i]);
	}
	printf("\n"); */
	

	return MPI_SUCCESS;
}
