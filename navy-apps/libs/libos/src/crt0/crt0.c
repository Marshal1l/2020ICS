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
  printf("ptr_argc:=%x\n", args);
  printf("argc:=%d\n", argc);
  exit(main(0, empty, empty));
  assert(0);
}
