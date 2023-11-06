#include <assert.h>
#include <mpi.h>
#include <math.h>

/* void MSTBcast(void *buffer, int count, MPI_Datatype datatype, int root, int rank, MPI_Comm comm, int left, int right){
	if (left == right)
	{
		return;
	}
	
	int mid = (left + right) / 2;
	int dest;
	if (root <= mid)
	{
		dest = right;
	} else {
		dest = left;
	}

	if (rank == root)
	{
		// send buffer to destination node
		MPI_Send(buffer, count, datatype, dest, 0, comm);
	} else if (rank == dest)
	{
		// receive buffer from root
		MPI_Recv(buffer, count, datatype, root, 0, comm, MPI_STATUS_IGNORE);
	}

	if (rank <= mid && root <= mid)
	{
		MSTBcast(buffer, count, datatype, root, rank, comm, left, mid);
	} else if (rank <= mid && root > mid)
	{
		MSTBcast(buffer, count, datatype, dest, rank, comm, left, mid);
	} else if (rank > mid && root <= mid)
	{
		MSTBcast(buffer, count, datatype, dest, rank, comm, mid + 1, right);
	} else if (rank > mid && root > mid)
	{
		MSTBcast(buffer, count, datatype, root, rank, comm, mid + 1, right);
	}
	
	
} */

int GT_Bcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm)
{
	assert(datatype == MPI_INT);
	assert(root == 0);
	
	/* Your code here (Do not just call MPI_Bcast) */
	int rank, size, mid, dest;
	MPI_Comm_rank(comm, &rank);
	MPI_Comm_size(comm, &size);

	int left = 0;
	int right = size - 1;

	while (left < right)
	{
		mid = floor((left + right) / 2);
		if (root <= mid)
		{
			dest = right;
		} else {
			dest = left;
		}
		
		if (rank == root)
		{
			// send buffer to destination node
			MPI_Send(buffer, count, datatype, dest, 0, comm);
		} else if (rank == dest)
		{
			// receive buffer from root
			MPI_Recv(buffer, count, datatype, root, 0, comm, MPI_STATUS_IGNORE);
		}

		if (rank <= mid && root <= mid)
		{
			right = mid;
		} else if (rank <= mid && root > mid)
		{
			right = mid;
			root = dest;
		} else if (rank > mid && root <= mid)
		{
			left = mid + 1;
			root = dest;
		} else if (rank > mid && root > mid)
		{
			left = mid + 1;
		}
	}
	
	return MPI_SUCCESS;
}
