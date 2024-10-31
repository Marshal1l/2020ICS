#include <common.h>
#include "syscall.h"
void do_syscall(Context *c)
{
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;

  switch (a[0])
  {
  case SYS_yield:
    printf("SYS_YIELD\n");
    yield();
    c->GPRx = 0;
    break;
  case SYS_exit:
    printf("SYS_EXIT\n");
    halt(a[0]);
    break;
  default:
    panic("Unhandled syscall ID = %d", a[0]);
  }
}
