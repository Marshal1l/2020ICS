#include <common.h>
void do_syscall(Context *c);
Context *schedule(Context *prev);
static Context *do_event(Event e, Context *c)
{
  switch (e.event)
  {
  case EVENT_SYSCALL:
    // printf("sys get SYSCALL!\n");
    do_syscall(c);
    break;
  case EVENT_YIELD:
    printf("sys get YEILD!\n");
    return schedule(c);
    break;
  case EVENT_ERROR:
    // printf("sys get ERROR!\n");
    break;
  default:
    panic("Unhandled event ID = %d", e.event);
  }
  return c;
}

void init_irq(void)
{
  Log("Initializing interrupt/exception handler...");
  cte_init(do_event);
}
