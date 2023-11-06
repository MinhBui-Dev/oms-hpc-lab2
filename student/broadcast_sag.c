#include <assert.h>
#include <mpi.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

void MSTScatter(void *sendbuf, void*recvbuf, int count, MPI_Datatype datatype, int root, int rank, MPI_Comm comm, int left, int right, int offset){
	if (left == right)
	{
		return;
	}

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
			MPI_Send(buffer_start, size, datatype, dest, 0, comm);
			if ((mid - left) == 0)
			{
				memcpy(recvbuf, (int*)sendbuf + (left_pos)*count, count*sizeof(int));
			}

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
			MPI_Send(buffer_start, size, datatype, dest, 0, comm);
			if ((right - mid) == 1)
			{
				memcpy(recvbuf, (int*)sendbuf + (mid_pos + 1 + offset)*count, count*sizeof(int));
			} 
			offset += mid - left + 1;

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
		MSTScatter(sendbuf, recvbuf, count, datatype, root, rank, comm, left, mid, offset);
	} else if (rank <= mid && root > mid)
	{
		MSTScatter(sendbuf, recvbuf, count, datatype, dest, rank, comm, left, mid, offset);
	} else if (rank > mid && root <= mid)
	{
		MSTScatter(sendbuf, recvbuf, count, datatype, dest, rank, comm, mid + 1, right, offset);
	} else if (rank > mid && root > mid)
	{
		MSTScatter(sendbuf, recvbuf, count, datatype, root, rank, comm, mid + 1, right, offset);
	}
	
	
}

int GT_Bcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm)
{
  assert(datatype == MPI_INT);
  assert(root == 0);
  
  /* Your code here (Do not just call MPI_Bcast) */
  int rank, size, prev, next;
  MPI_Comm_rank(comm, &rank);
  MPI_Comm_size(comm, &size);

  int left = 0;
  int right = size - 1;

  int subcount = count/size;
  void* recvbuf = malloc(subcount * sizeof(int));

  MSTScatter(buffer, recvbuf, subcount, datatype, root, rank, comm, left, right, 0);

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

  memcpy((int*)buffer + (rank)*subcount, recvbuf, subcount*sizeof(int));

  int pos = rank;
    
  for (int i = 0; i < size - 1; i++)
  {
    MPI_Request sendRequest, recvRequest;
    MPI_Isend((int*)buffer + (pos)*subcount, subcount, datatype, next, 0, comm, &sendRequest);
    pos = pos - 1;
    if (pos < 0)
    {
      pos = size - 1;
    }
    MPI_Irecv((int*)buffer + (pos)*subcount, subcount, datatype, prev, 0, comm, &recvRequest);
    MPI_Wait(&sendRequest, MPI_STATUS_IGNORE);
    MPI_Wait(&recvRequest, MPI_STATUS_IGNORE);   
  }

  free(recvbuf);
  
  return MPI_SUCCESS;
}
