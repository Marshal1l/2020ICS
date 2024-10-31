#include <common.h>

static Context *do_event(Event e, Context *c)
{
  switch (e.event)
  {
  case EVENT_SYSCALL:
    printf("sys get SYSCALL!\n");
    break;
  case EVENT_YIELD:
    printf("sys get YEILD!\n");
    break;
  case EVENT_ERROR:
    printf("sys get ERROR!\n");
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
