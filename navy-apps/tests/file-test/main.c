#include <stdio.h>
#include <assert.h>

int main()
{
  FILE *fp = fopen("/share/files/num", "r+");
  assert(fp);
  printf("stage open\n");
  fseek(fp, 0, SEEK_END);
  printf("stage 0\n");
  long size = ftell(fp);
  assert(size == 5000);
  printf("stage 1\n");
  fseek(fp, 500 * 5, SEEK_SET);
  printf("stage 2\n");
  int i, n;
  for (i = 500; i < 1000; i++)
  {
    fscanf(fp, "%d", &n);
    assert(n == i + 1);
  }
  printf("stage 3\n");
  fseek(fp, 0, SEEK_SET);
  for (i = 0; i < 500; i++)
  {
    fprintf(fp, "%4d\n", i + 1 + 1000);
  }
  printf("stage 4\n");
  for (i = 500; i < 1000; i++)
  {
    fscanf(fp, "%d", &n);
    assert(n == i + 1);
  }
  printf("stage 5\n");
  fseek(fp, 0, SEEK_SET);
  printf("stage 6\n");
  for (i = 0; i < 500; i++)
  {
    fscanf(fp, "%d", &n);
    assert(n == i + 1 + 1000);
  }
  printf("stage 7\n");
  fclose(fp);

  printf("PASS!!!\n");

  return 0;
}
