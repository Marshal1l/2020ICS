#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
int main(int argc, char *argv[], char *envp[]);
extern char **environ;
void call_main(uintptr_t *args)
{
  printf("-----------call main-----------\n");
  char *empty[] = {NULL};
  environ = empty;
  int argc = *args;
  args++;
  int envc = *args;
  args++;
  char *envp[10] = {};
  char *argv[10] = {};
  for (int i = 0; i < envc; i++)
  {
    envp[i] = *(char **)args;
    (char **)args++;
  }
  printf("argc:=%d\n", argc);
  printf("envc:=%d\n", envc);
  printf("envp 0:=%x\n", envp);
  exit(main(0, empty, empty));
  assert(0);
}
