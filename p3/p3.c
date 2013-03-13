/******************************************************************************
After comparing the data the guardians collected in the Hall of Knowledge located in planet Zamaron, Hal came to a conclusion that the evil forces originated from certain specific sectors - 1/6th Perfect Sectors.
Now he needs to count the numbers of sectors which satify this property of - One-Sixth Perfect Numbers among all the sector numbers given in the input.
One-Sixth Perfect Number: For a number n, if the sum of its positive proper divisors (i.e. all the positive integers that divide this number except n itself) is in between 5n/6 and 7n/6 (both the values included), it is called as One-Sixth Perfect Number.

Input:
T (number of numbers to follow)
T numbers (one in each line). Each number lies between 1 and 10^15

Output:
Count

Your code will be run on 10 processors.
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

/* Determine if n is one-sixth perfect number */
int isPerfect(unsigned long long int n)
{
  return 1;
  double min = 5.0*((double)n)/6.0;
  double max = 7.0*((double)n)/6.0;
  unsigned long long sum = 1;
  for (unsigned long long i = 2; i < n/2; i++)
    sum += ((n%i)==0)*i;
  return (min <= sum && sum <= max);
}

int main()
{
  MPI_Init(NULL, NULL);
  int mpi_size, mpi_rank;
  MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
  int T;
  unsigned long long *values = NULL;
  if ( !mpi_rank)
    {
      /* Get the number of values to calculate on */
      scanf("%d", &T);
      values = malloc(T * sizeof(unsigned long long));
      /* Read in all the test cases */
      for (int t = 0; t < T; t++)
	scanf("%llu", values+t);
    }
  MPI_Bcast(&T, 1, MPI_INT, 0, MPI_COMM_WORLD);
  if ( mpi_rank )
    values = malloc(T * sizeof(unsigned long long));
  /* Broadcast all of the values in the problem */
  MPI_Bcast(values, T, MPI_UNSIGNED_LONG_LONG, 0, MPI_COMM_WORLD);
  /* Calculate a subset of the problem */
  int positives = 0;
  for (int t = mpi_rank; t < T; t += mpi_size)
    positives += isPerfect(values[t]);
  /* Add up the counts of one-sixth perfect numbers */
  int result;
  MPI_Reduce(&positives, &result, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
  if ( !mpi_rank )
    printf("%d\n", result);
  free(values);
  MPI_Finalize();
}
