void init_monitor(int, char *[]);
void engine_start();
int is_exit_status_bad();
#include <stdio.h>
int main(int argc, char *argv[])
{
  for (int i = 0; argv[i] != NULL; i++)
    printf("%s\n", argv[i]);
  /* Initialize the monitor. */
  init_monitor(argc, argv);

  /* Start engine. */
  engine_start();

  return is_exit_status_bad();
}
