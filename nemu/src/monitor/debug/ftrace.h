#include <common.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <elf.h>
typedef struct
{
    char name[64];
    paddr_t addr; // the function head address
    Elf32_Xword size;
} Symbol;
