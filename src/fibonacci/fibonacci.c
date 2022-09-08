#include <stdio.h>

int getFibonacciNumer(int n)
{
  if (n == 1 || n == 2)
  {
    return 1;
  }

  return fibonacci(n-2) + fibonacci(n-1);
}

int* getFibonacciSequence(int n)
{
  int fibArray[n];

  for (int i = 1; i <= n; i++)
  {
    fibArray[i - 1] = getFibonacciNumer(i);
  }

  return fibArray;
}

int main(int argc, char *argv[])
{
  if (argc > 2)
  {
    printf("Too many arguments supplied");
  }

  if (argc == 3 && strcmp(argv[1], "s") == 0)
  {
    int nthTerm = argv[2];
    int* sequence = getFibonacciSequence(nthTerm);

    for (int i = 0; i < nthTerm; i++)
    {
      printf("%d ", sequence[i]);
    }

    printf("\n");

  }
  else if (argc == 2 && isdigit(argv[1]))
  {
    int nthTerm = argv[1];
    printf("%d", getFibonacciNumer(nthTerm));
  }
  else
  {
    printf("Usage instructions:\nThe program can be run in the following ways\n./fibonnaci s <some_number> will print the sequence of fibonacci numbers including the term that is requested\n./fibonacci <some_number> will print for you the nth number in the fibonacci sequence\n");
  }

  return 0;

}
