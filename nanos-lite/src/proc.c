#include <proc.h>

#define MAX_NR_PROC 4
void naive_uload(PCB *pcb, const char *filename);

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;
void switch_boot_pcb()
{
  current = &pcb_boot;
  printf("switch_boot_pcb current %d\n", current);
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
  context_kload(&pcb[0], hello_fun, NULL);
  printf("pcb0 %d\n", pcb[0].cp);
  switch_boot_pcb();

  // load program here
  // naive_uload(NULL, "/bin/menu");
}

Context *schedule(Context *prev)
{
  // save the context pointer
  current->cp = prev;
  // always select pcb[0] as the new process
  current = &pcb[0];
  // then return the new context
  return current->cp;
}
