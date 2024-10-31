#include <proc.h>
#include <elf.h>
#ifdef __LP64__
#define Elf_Ehdr Elf64_Ehdr
#define Elf_Phdr Elf64_Phdr
#else
#define Elf_Ehdr Elf32_Ehdr
#define Elf_Phdr Elf32_Phdr
#endif
extern size_t ramdisk_read(void *buf, size_t offset, size_t len);
extern size_t ramdisk_write(const void *buf, size_t offset, size_t len);

static uintptr_t loader(PCB *pcb, const char *filename)
{
  Elf_Ehdr ehdr;
  size_t ehdr_size = ramdisk_read(&ehdr, 0, sizeof(Elf_Ehdr));
  printf("ehdr_size=%lu\n", ehdr_size);
  return 0;
  // const char *file_name = filename;
  // file_name = "/home/mzh/2020PA/ics2020/nanos-lite/build/ramdisk.img";
  // if (file_name == NULL)
  // {
  //   assert(0);
  //   return 0;
  // }
  // int fd;
  // fd = fs_open(file_name, "rb");
  // if (fd == -1)
  // {
  //   assert(0);
  //   return 0;
  // }
  // Elf_Ehdr ehdr;
  // if (fs_read(&ehdr, sizeof(Elf32_Ehdr), 1, fd) <= 0)
  // {
  //   assert(0);
  //   return 0;
  // }
  // assert(*(uint32_t *)ehdr.e_ident == 0x7f454c46);
}

void naive_uload(PCB *pcb, const char *filename)
{
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void (*)())entry)();
}
