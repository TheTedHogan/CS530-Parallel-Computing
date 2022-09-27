#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <stdint.h>

double getFibonacciNumber(int n)
{
  //uint64_t num;
  double nume_1,nume_2,deno,num;
  //long long int num; 
  //uint64_t num;
  
  #pragma omp sections
  {
    #pragma omp section
      nume_1 = pow(((double)1+sqrt(5)),n);
    
    #pragma omp section
      nume_2 = pow(((double)1-sqrt(5)),n);

    #pragma omp section
      deno = (pow((double)2,n)*sqrt(5));
  }
  //printf("nume_1 is : %lf\nnume_2 is: %lf\ndeno is: %lf\n", nume_1,nume_2,deno);
  #pragma omp parallel
  num = (nume_1-nume_2)/deno;
  //printf("num is %lf\n",num);

  return num;

}

double* getFibonacciSequence(int n)
{
  double* fibArray = malloc(sizeof(double) * n);

  for (int i = 0; i <= n; i++)
  {
    fibArray[i] = getFibonacciNumber(i);
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
    double* sequence = getFibonacciSequence(nthTerm);

    for (int i = 0; i < nthTerm; i++)
    {
      printf("%.0f,", sequence[i]);
    }
    printf("\n");
  }
  else if (argc == 2 && isdigit(*argv[1]))
  {
    int nthTerm = atoi(argv[1]);
    printf("%.0f\n", getFibonacciNumber(nthTerm));
  }
  else
  {
    printf("Usage instructions:\nThe program can be run in the following ways\n./fibonnaci <some_number> s will print the sequence of fibonacci numbers including the term that is requested\n./fibonacci <some_number> will print for you the nth number in the fibonacci sequence\n");
    exit(1);
  }

  return 0;

}
