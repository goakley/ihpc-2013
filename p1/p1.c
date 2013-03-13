/*******************************************************************************
Given two vectors of N dimension output the vector sum.

Input:
The first line contains an integer T (1<=T<=100), the number of test cases. The second line contains an integer, n (1<=n<=1000000), the dimension of the vectors. It is followed by two lines containing n space-separated integers each, representing the vector A and B to be added.

Output:
On a single line, output n space separated integers representing the vector A+B. Note: Print a newline between the result of test cases.

Sample Input:
2
4
1 1 2 3
2 3 4 1
5
1 2 3 4 5
5 4 3 2 1

Sample Output:
3 4 6 4
6 6 6 6 6

USE MPI. Your code must be designed to run on 16 processes using MPI.
******************************************************************************/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>



// MASTER CREATED DATA
int *vectors;       // The vector data entered by the user
int T; // 1 -> 100     The number of vector sums that must be computed
int n_indices[100]; // The starting indices in 'vectors' of each T-event
int n_lengths[100]; // The length in 'vectors' of each T-event


int slave(int rank, int orig_size)
{
  int crop_size = orig_size/2;
  int *vecs = malloc(orig_size*sizeof(int));
  MPI_Recv(vecs, orig_size, MPI_INT, 0, 1, MPI_COMM_WORLD, NULL);
  for (int i = 0; i < orig_size; i += 2)
    vecs[crop_size] = vecs[i]+vecs[i+1];
  MPI_Send(vecs, crop_size, MPI_INT, 0, rank, MPI_COMM_WORLD);
  free(vecs);
}



/* MASTER - Read a T-event into the specified array and set the length of the 
 *          data read in */
int MASTER_readt(int *vector, int *length)
{
  scanf("%d", length);
  *length = (*length)*2;
  int l = *length;
  // scan in vector t1
  for (int i = 0; i < l; i += 2)
    scanf("%d", vector+i);
  for (int i = 1; i < l; i += 2)
    scanf("%d", vector+i);
}

/* MASTER - Initialize the array of vectors by filling the array with the 
 *          appropriate data and setting its length */
int MASTER_init(int *vectors, int *length,
		int *T, int *n_indices, int *n_lengths)
{
  // Read in the number of vector pairs to calculate
  scanf("%d",T);
  // Read in each individual T-event
  int index = 0;
  // Iterate through each T-event, reading in its data
  for (int t = 0; t < *T; t++)
    {
      // Store the starting index of the T-event
      n_indices[t] = index;
      // Read in the data for the T-event
      MASTER_readt(vectors+index, n_lengths+t);
      // Set the new index to be the previous index plus the entry's length
      index += n_lengths[t];
    }
  // Store the length of the entire vector
  *length = index;
  // Shrink the vector to its length (to save memory)
  vectors = realloc(vectors, (*length)*sizeof(int));
}

int MASTER_deinit(int *vectors, int T, int *n_indices, int *n_lengths)
{
  int index = 0;
  for (int t = 0; t < T; t++)
    {
      for (int i = 0; i < n_lengths[t]/2; i++)
	printf("%d ", vectors[index+i]);
      index += n_lengths[t]/2;
      putchar('\n');
    }
}

int main()
{
  MPI_Init(NULL, NULL);
  int mpi_size, mpi_rank;
  MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
  int *vectors = NULL;// The entirety of the vector data
  int T;              // The number of vector sums that must be computed
  int n_indices[100]; // The starting indices in 'vectors' of each T-event
  int n_lengths[100]; // The length in 'vectors' of each T-event
  int length_global = 0;
  if ( !mpi_rank )
    { // Have the MASTER read in the vector data
      // Make room to store the maximum possible number of vector entries
      vectors = malloc(1000000*100*2*sizeof(int)); // max(n)*max(T)*2
      // Completely prepare the array of vectors for use
      MASTER_init(vectors, &length_global, &T, n_indices, n_lengths);
    }
  // Broadcast the total vector data length, allowing slaves to 
  // compute their length
  MPI_Bcast(&length_global, 1, MPI_INT, 0, MPI_COMM_WORLD);
  // Get a count of the number of adds that must be performed (globally)
  int addcount_global = (length_global/2);
  int spillover = addcount_global % mpi_size;
  // Get a count of the number of local adds that must be performed
  int addcount_local = addcount_global/mpi_size + (mpi_rank < spillover);
  // Calculate the length of the local vectors array;
  int length_local = addcount_local*2;
  if ( mpi_rank )
    { // Allocate memory for the SLAVES to store data
      vectors = malloc(length_local*sizeof(int));
      MPI_Recv(vectors, length_local, MPI_INT, 0, 0, MPI_COMM_WORLD, NULL);
      for (int i = 0; i < addcount_local; i++)
	vectors[i] = vectors[i*2] + vectors[i*2+1];
      MPI_Send(vectors, addcount_local, MPI_INT, 0, mpi_rank, MPI_COMM_WORLD);
    }
  else
    {
      // Send out the work data
      int index = ((addcount_global/mpi_size)+(0<spillover))*2;
      for (int i = 1; i < mpi_size; i++) {
	int length = ((addcount_global/mpi_size)+(i<spillover))*2;
	MPI_Send(vectors+index, length, MPI_INT, i, 0, MPI_COMM_WORLD);
	index += length;
      }
      // Perform the local work
      for (int i = 0; i < addcount_local; i++)
	vectors[i] = vectors[i*2] + vectors[i*2+1];
      // Collect the work result
      index = (addcount_global/mpi_size)+(0<spillover);
      for (int i = 1; i < mpi_size; i++) {
	int length = (addcount_global/mpi_size)+(i<spillover);
	MPI_Recv(vectors+index, length, MPI_INT, i, i, MPI_COMM_WORLD, NULL);
	index += length;
      }
      // Output the result
      MASTER_deinit(vectors, T, n_indices, n_lengths);
    }
  free(vectors);
  MPI_Finalize();
  return 0;
}
