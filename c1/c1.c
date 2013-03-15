/******************************************************************************
The Story so far - The Green Lantern Corps is the largest intergalactic peace-keeping force. They patrol the farthest reaches of the Universe at the behest of the Guardians(MASTER NODES), a race of immortals residing on the planet Oa. The Green Lantern Corp's operation is divided as pairs amongst the 3600 'sectors' (NODES) of the universe, over 7200 members (known commonly as Green Lanterns)(Processors) are estimated to be serving within the Corps. Each Green Lantern is given a power ring, a weapon granting the use of incredible abilities that are directed by the wearer's own willpower.
A sudden attack by a powerful force brought in the 'Blackest Night' where both the Corps and the Guardians have suffered huge casualties. The Guardians' failure to take appropriate action during the Crisis has led to the survival of the last remaining guardian YOU - Sodam Yat. Left to their own devices, the Corps underwent a major reorganization and now you have to lead them. A team of Green Lanterns brought the new recruit Hal Jordan from Earth. You, Sodam Yat see a great potential in this green lantern, maybe one day he might save the Universe. So you design special tasks to train him to become a guardian one day.
These task are specially designed so that he couldn't complete it alone. He need to show creativity, teamwork and leadership.

His first task was to find the cause of this crisis from the traces of matter found after the war by reading and comparing with the data the guardians collected in the Hall of Knowledge located in planet Zamaron.

In his second task, after comparing the data the guardians collected in the Hall of Knowledge located in planet Zamaron, Hal came to a conclusion that the evil forces originated from certain specific sectors.

The coordinates of those sectors are given in N dimension vector notation along with initial starting coordinate. Hal's third task is to find the vectorial distance.

Given two vectors of N dimension output the difference of the two vectors.

Input:
The first line contains an integer T (1<=T<=100), the number of test cases. The second line contains an integer, n (1<=n<=1000000), the dimension of the vectors. It is followed by two lines containing n space-separated integers each, representing the vector A and B to be added.

Output:
On a single line, output n space separated integers representing the vector B-A. Note: Print a newline between the result of test cases.

Sample Input:
2
4
1 1 2 3
2 3 4 1
5
1 2 3 4 5
5 4 3 2 1

Sample Output:
1 2 2 -2
4 2 0 -2 -4

Your code must be designed to run on 10 processes using MPI.

C1 Scoring : 500(correct submission) + 5*[speed factor]
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main()
{
  MPI_Init(NULL, NULL);
  int mpi_size, mpi_rank;
  MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
  int test_case_count; // Number of vector pairs in the problem
  int *test_case_vector_sizes = NULL; // Array holding each vector(-pair) size
  int *test_case_vectors = NULL; // The vectors themselves, adjacently stored
  int total_vectors_size = 0; // ~Sum of 'test_case_vector_sizes'
  // MASTER - read in the data
  if ( !mpi_rank )
    {
      scanf("%d", &test_case_count);
      test_case_vector_sizes = malloc(test_case_count*sizeof(int));
      for (int t = 0; t < test_case_count; t++)
	{
	  int vector_size; // the size of vector 't'
	  scanf("%d", &vector_size);
	  // increase the size of the vectors array to hold the new data
	  test_case_vectors = 
	    realloc(test_case_vectors,
		    (total_vectors_size+vector_size)*2*sizeof(int));
	  // read in the 1st vector, skipping every other index (2nd vector)
	  for (int i = 0; i < vector_size; i++)
	    scanf("%d", test_case_vectors + (total_vectors_size+i)*2);
	  // read in the 2nd vector, storing next to the 1st vector's entries
	  for (int i = 0; i < vector_size; i++)
	    scanf("%d", test_case_vectors + (total_vectors_size+i)*2+1);
	  test_case_vector_sizes[t] = vector_size;
	  total_vectors_size += vector_size;
	}
    }
  MPI_Bcast(&total_vectors_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
  int calculation_count = 
    total_vectors_size/mpi_size + (total_vectors_size%mpi_size != 0);
  // SLAVE - make room for vector data
  if ( mpi_rank)
    {
      test_case_vectors = malloc(calculation_count*2*sizeof(int));
    }
  MPI_Scatter(test_case_vectors, calculation_count*2, MPI_INT, 
	      test_case_vectors, calculation_count*2, MPI_INT, 
	      0, MPI_COMM_WORLD);
  for (int i = 0; i < calculation_count; i++)
    test_case_vectors[i] = test_case_vectors[(i*2)+1] - test_case_vectors[i*2];
  MPI_Gather(test_case_vectors, calculation_count, MPI_INT, 
	     test_case_vectors, calculation_count, MPI_INT, 
	     0, MPI_COMM_WORLD);
  if ( !mpi_rank )
    {
      int index = 0;
      for (int t = 0; t < test_case_count; t++)
	{
	  int size = test_case_vector_sizes[t];
	  int i = 0;
	  for (; i < size; i++)
	    printf("%d ", test_case_vectors[index+i]);
	  putchar('\n');
	  index += i;
	}
    }
  free(test_case_vector_sizes);
  free(test_case_vectors);
  MPI_Finalize();
}
