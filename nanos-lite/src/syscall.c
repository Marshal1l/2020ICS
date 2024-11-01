#include <common.h>
#include "syscall.h"
extern int fs_open(const char *pathname, int flags, int mode);
extern size_t fs_read(int fd, void *buf, size_t len);
extern size_t fs_write(int fd, const void *buf, size_t len);
extern size_t fs_lseek(int fd, size_t offset, int whence);
extern int fs_close(int fd);
void do_syscall(Context *c)
{
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;
  switch (a[0])
  {
  case SYS_brk:
    // printf("SYS_BRK------%d\n", SYS_brk);
    c->GPRx = 0;
    break;
  case SYS_open:
    c->GPRx = fs_open((const char *)a[1], a[2], a[3]);
    break;
  case SYS_close:
    c->GPRx = fs_close(a[1]);
    break;
  case SYS_lseek:
    c->GPRx = fs_lseek(a[1], (size_t)a[2], a[3]);
    break;
  case SYS_read:
    c->GPRx = fs_read(a[1], (void *)a[2], a[3]);
    break;
  case SYS_write:
    c->GPRx = fs_write(a[1], (const void *)a[2], a[3]);
    break;
  case SYS_yield:
    // printf("SYS_YIELD------%d\n", SYS_yield);
    yield();
    c->GPRx = 1;
    break;
  case SYS_exit:
    // printf("SYS_EXIT------%d\n", SYS_exit);
    halt(a[0]);
    break;
  default:
    panic("Unhandled syscall ID = %d", a[0]);
  }
}
