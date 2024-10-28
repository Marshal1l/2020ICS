#include <common.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <elf.h>
void init_ftrase(const char *elf_file)
{
    int fd = open(elf_file, O_RDONLY);
    if (fd < 0)
    {
        fprintf(stderr, "Usage: %s <elf-file>\n", elf_file);
        // exit(1);
    }
    Elf64_Ehdr ehdr;
    if (read(fd, &ehdr, sizeof(ehdr)) != sizeof(ehdr))
    {
        fprintf(stderr, "read ehdr\n");
    }
    // off_t shstrndx;
    if (ehdr.e_shstrndx >= ehdr.e_shnum)
    {
        fprintf(stderr, "Invalid section header string table index\n");
    }
    close(fd);
}