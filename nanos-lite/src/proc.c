#include <proc.h>

#define MAX_NR_PROC 4
void naive_uload(PCB *pcb, const char *filename);

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;
void switch_boot_pcb()
{
  current = &pcb_boot;
}

void hello_fun(void *arg)
{
  int j = 1;
  while (1)
  {
    Log("Hello World from Nanos-lite with arg '%p' for the %dth time!", (uintptr_t)arg, j);
    j++;
    yield();
  }
}
extern void context_kload(PCB *pcb, void (*entry)(void *), void *arg);
void init_proc()
{
  // switch_boot_pcb();

  Log("Initializing processes...");
  context_kload(&pcb[1], hello_fun, NULL);

  switch_boot_pcb();
  // printf("pcb1 %d\n", pcb[1].cp);
  // ((void (*)())(pcb[1].cp->eip))();
  // load program here
  // naive_uload(NULL, "/bin/menu");
}

Context *schedule(Context *prev)
{
  return NULL;
}
