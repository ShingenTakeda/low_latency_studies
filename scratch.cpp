#include <cstdio>
#include <cstdlib>

void loop_without_pointer_aliasing(int *__restrict a, int *__restrict b, int n)
{
  for (int i = 0; i < n; i++)
  {
    a[i] = *b;
  }

  for (int i = 0; i < n; i++)
  {
    printf("%i", a[i]);
  }

  printf("\n");
}

int main()
{
  int *a = (int *)malloc(sizeof(int) * 10);
  int b = 5;

  loop_without_pointer_aliasing(a, &b, 10);

  return 0;
}
