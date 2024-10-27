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
    close(fd);
}