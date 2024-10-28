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
        assert(0);
    }
    Elf64_Ehdr ehdr;
    if (read(fd, &ehdr, sizeof(ehdr)) != sizeof(ehdr))
    {
        fprintf(stderr, "read ehdr failed\n");
        assert(0);
    }
    if (ehdr.e_ident[0] != 0x7f || ehdr.e_ident[1] != 'E' ||
        ehdr.e_ident[2] != 'L' || ehdr.e_ident[3] != 'F')
    {
        printf("The opened file isn't a elf file!\n");
        exit(0);
    }
    if (ehdr.e_shstrndx >= ehdr.e_shnum)
    {
        fprintf(stderr, "Invalid section header string table index\n");
        assert(0);
    }
    close(fd);
}