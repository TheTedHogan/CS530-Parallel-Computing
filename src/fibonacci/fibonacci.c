#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

int getFibonacciNumber(int n)
{
  if (n == 1 || n == 2)
  {
    return 1;
  }

  return getFibonacciNumber(n-2) + getFibonacciNumber(n-1);
}

int* getFibonacciSequence(int n)
{
  int* fibArray = malloc(n);

  for (int i = 1; i <= n; i++)
  {
    fibArray[i - 1] = getFibonacciNumber(i);
  }

  return fibArray;
}

int main(int argc, char *argv[])
{
  if (argc > 3)
  {
    printf("Too many arguments supplied\n");
  }

  if (argc == 3 && strcmp(argv[2], "s") == 0)
  {
    int nthTerm = atoi(argv[1]);
    int* sequence = getFibonacciSequence(nthTerm);

    for (int i = 0; i < nthTerm; i++)
    {
      printf("%d ", sequence[i]);
    }
    printf("\n");
  }
  else if (argc == 2 && isdigit(*argv[1]))
  {
    int nthTerm = atoi(argv[1]);
    printf("%d\n", getFibonacciNumber(nthTerm));
  }
  else
  {
    printf("Usage instructions:\nThe program can be run in the following ways\n./fibonnaci s <some_number> will print the sequence of fibonacci numbers including the term that is requested\n./fibonacci <some_number> will print for you the nth number in the fibonacci sequence\n");
  }

  return 0;

}
