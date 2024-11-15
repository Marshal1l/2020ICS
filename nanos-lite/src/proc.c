#include <proc.h>

#define MAX_NR_PROC 4
void naive_uload(PCB *pcb, const char *filename);

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;
void switch_boot_pcb()
{
  current = &pcb_boot;
  printf("switch_boot_pcb current %x\n", current);
}

void hello_fun(void *arg)
{
  int j = 1;
  while (1)
  {
    Log("Hello World from Nanos-lite with arg '%s' for the %dth time!", (char *)arg, j);
    j++;
    yield();
  }
}
extern void context_uload(PCB *pcb, const char *filename, char *const argv[], char *const envp[]);
extern void context_kload(PCB *pcb, void (*entry)(void *), void *arg);
void init_proc()
{
  switch_boot_pcb();

  Log("Initializing processes...");
  // context_kload(&pcb[0], hello_fun, "A");
  // context_kload(&pcb[1], hello_fun, "V");
  char *argvs[] = {NULL};
  char *envs[] = {NULL};
  context_uload(&pcb[0], "/bin/hello", argvs, envs);
  switch_boot_pcb();

  // load program here
  // naive_uload(NULL, "/bin/menu");
}

Context *schedule(Context *prev)
{
  // save the context pointer
  current->cp = prev;
  // always select pcb[0] as the new process
  // current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);
  current = &pcb[0];
  // then return the new context
  return current->cp;
}
int sys_execve(const char *filename, char *const argv[], char *const envp[])
{
  context_uload(current, filename, argv, envp);
  // printf("%s %s %s\n",filename,argv[0],envp[0]);
  switch_boot_pcb();
  // printf("has loaded\n");
  yield();
  // naive_uload(NULL,filename);
  return 0;
}