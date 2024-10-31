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
  printf("ehdr_size=%d\n", ehdr_size);
  // printf("ehdr_e_ident=%d\n", *(uint32_t *)ehdr.e_ident);
  assert(*(uint32_t *)(ehdr.e_ident) == 0x464c457f);
  Elf_Phdr phdr;
  for (int i = 0; i < ehdr.e_phnum; i++)
  {
    printf("1\n");
    ramdisk_read(&phdr, ehdr.e_phoff, sizeof(Elf_Phdr));
    if (phdr.p_type == PT_LOAD)
    {
      printf("read=%d\n", phdr.p_type);
      uint32_t v_s = phdr.p_vaddr;
      ramdisk_read((uint8_t *)v_s, phdr.p_offset, phdr.p_filesz);
      memset((uint8_t *)(v_s + phdr.p_filesz), 0, (phdr.p_memsz - phdr.p_filesz));
    }
  }
  return 0;
}

void naive_uload(PCB *pcb, const char *filename)
{
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void (*)())entry)();
}
