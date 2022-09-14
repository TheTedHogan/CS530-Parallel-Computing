#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

int main()
{
  if (system("./fibonacci -1\n") == 0)
  {
    printf("Input is -1\n");
    printf("FAILED: didn't catch negative value!\n");
  }
  else
  {
    printf("Input is -1\n");
    printf("SUCCESS: caught negative value!\n");
  }

  if (system("./fibonacci -0.1\n") == 0)
  {
    printf("Input is -0.1\n");
    printf("FAILED: didn't catch negative decimal value!\n");
  }
  else
  {
    printf("Input is -0.1\n");
    printf("SUCCESS: caught negative decimal value!\n");
  }

  if (system("./fibonacci 0\n") == 0)
  {
    printf("Input is 0\n");
    printf("FAILED: didn't catch 0 value!\n");
  }
  else
  {
    printf("Input is 0\n");
    printf("SUCCESS: caught 0 value!\n");
  }

  if (system("./fibonacci 0.1\n") == 0)
  {
    printf("Input is 0.1\n");
    printf("FAILED: didn't catch decimal value!\n");
  }
  else
  {
    printf("Input is 0.1\n");
    printf("SUCCESS: caught decimal value!\n");
  }
  if (system("./fibonacci 3 4 5 s") == 0)
  {
    printf("FAILED: didn't catch too many args!\n");
  }
  else
  {
    printf("SUCCESS: caught too many arg values!\n");
  }
  if (system("./fibonacci") == 0)
  {
    printf("FAILED: didn't catch not enough args!\n");
  }
  else
  {
    printf("SUCCESS: caught not enough args!\n");
  }
  if (system("./fibonacci s") == 0)
  {
    printf("FAILED: didn't catch non digit first arg!\n");
  }
  else
  {
    printf("SUCCESS: caught non digit first arg!\n");
  }
  return 0;
}
