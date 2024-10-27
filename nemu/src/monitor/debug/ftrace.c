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
        exit(EXIT_FAILURE);
    }

    close(fd);
}