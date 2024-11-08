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
  printf("load file-- %s\n", filename);
  file_id = fs_open(filename, 0, 0);
  Elf_Ehdr ehdr;
  fs_read(file_id, &ehdr, sizeof(Elf_Ehdr));
  // printf("ehdr_size=%d\n", ehdr_size);
  // printf("ehdr_phnum=%d\n", ehdr.e_phnum);
  // printf("ehdr_e_ident=%s\n", ehdr.e_ident);
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
  return ehdr.e_entry;
}

extern Area heap;
// static char *argv_tem[10] = {NULL};

void context_kload(PCB *pcb, void (*entry)(void *), void *arg)
{
  Area stack;
  stack.start = pcb->stack;
  stack.end = pcb->stack + STACK_SIZE;
  pcb->cp = kcontext(stack, entry, arg);
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