#include <proc.h>
#include <elf.h>
#include <unistd.h>
#ifdef __LP64__
#define Elf_Ehdr Elf64_Ehdr
#define Elf_Phdr Elf64_Phdr
#else
#define Elf_Ehdr Elf32_Ehdr
#define Elf_Phdr Elf32_Phdr
#endif
#define PGSIZE 4096
#define stack_num 8
extern void *new_page(size_t);
extern Context *ucontext(AddrSpace *as, Area kstack, void *entry);
extern Context *kcontext(Area kstack, void (*entry)(void *), void *arg);
extern int fs_open(const char *pathname, int flags, int mode);
extern size_t fs_read(int fd, void *buf, size_t len);
extern size_t fs_write(int fd, const void *buf, size_t len);
extern size_t fs_lseek(int fd, size_t offset, int whence);
extern int fs_close(int fd);
//
extern size_t ramdisk_read(void *buf, size_t offset, size_t len);
extern size_t ramdisk_write(const void *buf, size_t offset, size_t len);
#define DEFAULT_ENTRY ((void *)0x3000000)
static uintptr_t loader(PCB *pcb, const char *filename)
{
  int file_id = -1;
  file_id = fs_open(filename, 0, 0);
  Elf_Ehdr ehdr;
  fs_read(file_id, &ehdr, sizeof(Elf_Ehdr));
  assert(*(uint32_t *)(ehdr.e_ident) == 0x464c457f);
  Elf_Phdr phdr;
  for (int i = 0; i < ehdr.e_phnum; i++)
  {
    fs_lseek(file_id, ehdr.e_phoff + i * sizeof(Elf_Phdr), SEEK_SET);
    fs_read(file_id, &phdr, sizeof(Elf_Phdr));
    if (phdr.p_type == PT_LOAD)
    {
      fs_lseek(file_id, phdr.p_offset, SEEK_SET);
      fs_read(file_id, (void *)phdr.p_vaddr, phdr.p_filesz);
      memset((void *)(phdr.p_vaddr + phdr.p_filesz), 0, (phdr.p_memsz - phdr.p_filesz));
    }
  }
  printf("load file-- %s\tentry--%x\tentry--%d\n", filename, ehdr.e_entry, ehdr.e_entry);
  return ehdr.e_entry;
}

extern Area heap;
// static char *argv_tem[10] = {NULL};

void context_kload(PCB *pcb, void (*entry)(void *), void *arg)
{
  // printf("context Kload entry = %d\n", entry);
  Area stack;
  stack.start = pcb->stack;
  stack.end = pcb->stack + sizeof(pcb->stack);
  pcb->cp = kcontext(stack, entry, arg);
}
void context_uload(PCB *pcb, const char *filename, char *const argv[], char *const envp[])
{
  Area stack;
  stack.start = pcb->stack;
  stack.end = pcb->stack + sizeof(pcb->stack);
  uintptr_t entry = loader(pcb, filename);
  int size_argv = 0, size_envp = 0, argc = 0, envc = 0;
  void *start = new_page(stack_num);
  void *end = start + stack_num * PGSIZE;
  Area ustack = {start, end};
  uintptr_t *user_stack = ustack.end;
  uintptr_t *ptr_argv = user_stack;
  while (argv[argc] != NULL)
  {
    // printf("arg %d:=%s\n", argc, argv[argc]);
    size_argv += strlen(argv[argc]) + 1;
    argc++;
  }
  while (envp[envc] != NULL)
  {
    size_envp += strlen(envp[envc]) + 1;
    envc++;
  }
  // printf("argc:=%d\tenvc:=%d\n", argc, envc);
  for (int i = argc - 1; i >= 0; i--)
  {
    size_t len = strlen(argv[i]) + 1; // 包括 '\0' 终止符
    user_stack -= len;
    strncpy((char *)user_stack, argv[i], len);
  }
  uintptr_t *ptr_envp = user_stack;
  for (int i = envc - 1; i >= 0; i--)
  {
    size_t len = strlen(envp[i]) + 1; // 包括 '\0' 终止符
    user_stack -= len;
    strncpy((char *)user_stack, envp[i], len);
  }
  // printf("ptr_argv:=%x\n", ptr_argv);
  // printf("ptr_envp:=%x\n", ptr_envp);
  // user_stack -= strlen("NULL");
  // uintptr_t *ptr_null = user_stack;
  // strncpy((char *)user_stack, "NULL", strlen(NULL) + 1);
  for (int i = argc - 1; i >= 0; i--)
  {
    (uintptr_t **)user_stack--;
    ptr_argv -= (strlen(argv[i]) + 1);
    *(uintptr_t **)user_stack = ptr_argv;
  }
  for (int i = envc - 1; i >= 0; i--)
  {
    (uintptr_t **)user_stack--;
    ptr_envp -= (strlen(envp[i]) + 1);
    *(uintptr_t **)user_stack = ptr_envp;
  }
  user_stack--;
  *user_stack = envc;
  user_stack--;
  *user_stack = argc;
  // return
  // printf("ustack addr:=%x\n", (uintptr_t)user_stack);
  // for (uintptr_t *i = (uintptr_t *)user_stack; i != (uintptr_t *)ustack.end; i++)
  // {
  //   printf("%x\n", *i);
  // }

  pcb->cp = ucontext(NULL, stack, (void *)entry);
  pcb->cp->eax = (uintptr_t)user_stack;
}
void naive_uload(PCB *pcb, const char *filename)
{
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void (*)())entry)();
}
int sys_execve(const char *filename, char *const argv[], char *const envp[])
{
  naive_uload(NULL, filename);
  return -1;
}