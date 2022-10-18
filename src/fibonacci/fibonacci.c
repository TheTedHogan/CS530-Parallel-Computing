#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

int getFibonacciNumber(int n)
{
  if ((double) ceilf(n) / (double) floorf(n) != 1)
  {
    printf("Input must be an integer\n");
    exit(1);
  }

  if(n < 1)
  {
    exit(1);
  }

  if (n == 1 || n == 2)
  {
    return 1;
  }

  return getFibonacciNumber(n-2) + getFibonacciNumber(n-1);
}

int* getFibonacciSequence(int n)
{
  int* fibArray = malloc(sizeof(int) * n);

  for (int i = 1; i <= n; i++)
  {
    fibArray[i - 1] = getFibonacciNumber(i);
  }

  return fibArray;
}

int main(int argc, char *argv[])
{
  if (argc > 3 || argc < 2)
  {
    printf("Usage instructions:\nThe program can be run in the following ways\n./fibonnaci <some_number> s will print the sequence of fibonacci numbers including the term that is requested\n./fibonacci <some_number> will print for you the nth number in the fibonacci sequence\n");
    exit(1);
  }

  if (!isdigit(*argv[1]))
  {
    fprintf(stderr, "First argument must be an integer\n");
    exit(1);
  }

  if (argc == 3 && strcmp(argv[2], "s") == 0)
  {
    int nthTerm = atoi(argv[1]);
    int* sequence = getFibonacciSequence(nthTerm);

    // for (int i = 0; i < nthTerm; i++)
    // {
    //   printf("%d ", sequence[i]);
    // }
    // printf("\n");
  }
  else if (argc == 2 && isdigit(*argv[1]))
  {
    // int nthTerm = atoi(argv[1]);
    // printf("%d\n", getFibonacciNumber(nthTerm));
  }
  else
  {
    printf("Usage instructions:\nThe program can be run in the following ways\n./fibonnaci <some_number> s will print the sequence of fibonacci numbers including the term that is requested\n./fibonacci <some_number> will print for you the nth number in the fibonacci sequence\n");
    exit(1);
  }

  return 0;

}
