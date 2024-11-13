#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
int main(int argc, char *argv[], char *envp[]);
extern char **environ;
void call_main(uintptr_t *args)
{
  printf("-----------call main-----------\n");
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
    // printf("envp%d:=%s\n", i, envp[i]);
  }
  for (int i = 0; i < envc; i++)
  {
    argv[i] = *(char **)args;
    (char **)args++;
    // printf("argv%d:=%s\n", i, argv[i]);
  }
  // printf("argc:=%d\n", argc);
  // printf("envc:=%d\n", envc);
  argv[argc] = NULL;
  envp[envc] = NULL;
  exit(main(0, argv, envp));
  assert(0);
}
