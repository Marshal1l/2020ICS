#include <am.h>
#include <x86.h>
#include <klib.h>

#define NR_IRQ 256 // IDT size
#define SEG_KCODE 1
#define SEG_KDATA 2

static Context *(*user_handler)(Event, Context *) = NULL;

void __am_irq0();
void __am_vecsys();
void __am_vectrap();
void __am_vecnull();

Context *__am_irq_handle(Context *c)
{
  // printf("c->eflags=\t%d\n", c->eflags);
  // printf("c->cs=\t%d\n", c->cs);
  // printf("c->eip=\t%d\n", c->eip);
  // printf("c->error_code=\t%x\n", c->error_code);
  printf("c->irq=\t%d\n", c->irq);
  // printf("c->eax=\t%d\n", c->eax);
  // printf("c->ecx=\t%d\n", c->ecx);
  // printf("c->edx=\t%d\n", c->edx);
  // printf("c->ebx=\t%d\n", c->ebx);
  // printf("c->esp=\t%d\n", c->esp);
  // printf("c->ebp=\t%d\n", c->ebp);
  // printf("c->esi=\t%d\n", c->esi);
  // printf("c->edi=\t%d\n", c->edi);
  if (user_handler)
  {
    Event ev = {
        .event = EVENT_NULL,
        .cause = 0,
        .ref = 0,
        .msg = "(no message)",
    };
    switch (c->irq)
    {
    case EVENT_YIELD:
      ev.event = EVENT_YIELD;
      break;
    case EVENT_ERROR:
      ev.event = EVENT_ERROR;
      break;
    default:
      ev.event = EVENT_ERROR;
      break;
    }

    c = user_handler(ev, c);
    assert(c != NULL);
  }

  return c;
}

bool cte_init(Context *(*handler)(Event, Context *))
{
  static GateDesc32 idt[NR_IRQ];

  // initialize IDT
  for (unsigned int i = 0; i < NR_IRQ; i++)
  {
    idt[i] = GATE32(STS_TG, KSEL(SEG_KCODE), __am_vecnull, DPL_KERN);
  }

  // ----------------------- interrupts ----------------------------
  idt[32] = GATE32(STS_IG, KSEL(SEG_KCODE), __am_irq0, DPL_KERN);
  // ---------------------- system call ----------------------------
  idt[0x80] = GATE32(STS_TG, KSEL(SEG_KCODE), __am_vecsys, DPL_USER);
  idt[0x81] = GATE32(STS_TG, KSEL(SEG_KCODE), __am_vectrap, DPL_KERN);

  set_idt(idt, sizeof(idt));

  // register event handler
  user_handler = handler;

  return true;
}

Context *kcontext(Area kstack, void (*entry)(void *), void *arg)
{
  return NULL;
}

void yield()
{
  asm volatile("int $0x81");
}

bool ienabled()
{
  return false;
}

void iset(bool enable)
{
}
