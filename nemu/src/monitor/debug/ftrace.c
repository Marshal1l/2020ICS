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
    Elf64_Xword size;
} Symbol;
Symbol *symbol = NULL;
static int func_num = 0;
const char *check_func(vaddr_t insta_addr)
{
    Symbol *tmpsymbol = symbol;
    while (tmpsymbol != NULL)
    {
        if (insta_addr >= tmpsymbol->addr && insta_addr < tmpsymbol->addr + tmpsymbol->size)
        {
            return (const char *)tmpsymbol->name;
        }
    }
    return "???";
}
void init_ftrase(const char *elf_file)
{
    if (elf_file == NULL)
    {
        return;
    }
    FILE *fd;
    fd = fopen(elf_file, "rb");
    if (fd == NULL)
    {
        fprintf(stderr, "Usage: %s <elf-file>\n", elf_file);
        assert(0);
    }
    Elf64_Ehdr ehdr;
    if (fread(&ehdr, sizeof(ehdr), 1, fd) != sizeof(ehdr))
    {
        fprintf(stderr, "read ehdr failed\n");
        assert(0);
    }
    if (ehdr.e_ident[0] != 0x7f || ehdr.e_ident[1] != 'E' ||
        ehdr.e_ident[2] != 'L' || ehdr.e_ident[3] != 'F')
    {
        printf("The opened file isn't a elf file!\n");
        assert(0);
    }
    fseek(fd, ehdr.e_shoff, SEEK_SET);
    Elf64_Shdr shdr;
    char *string_table = NULL;
    for (int i = 0; i < ehdr.e_shnum; i++)
    {
        if (fread(&shdr, sizeof(Elf64_Shdr), 1, fd) <= 0)
        {
            printf("fail to read the shdr\n");
            assert(0);
        }
        if (shdr.sh_type == SHT_STRTAB)
        {
            // get string table
            string_table = malloc(shdr.sh_size);
            fseek(fd, shdr.sh_offset, SEEK_SET);
            if (fread(string_table, shdr.sh_size, 1, fd) <= 0)
            {
                printf("fail to read the strtab\n");
                exit(0);
            }
        }
    }
    fseek(fd, ehdr.e_shoff, SEEK_SET);
    for (int i = 0; i < ehdr.e_shnum; i++)
    {
        if (fread(&shdr, sizeof(Elf64_Shdr), 1, fd) <= 0)
        {
            printf("fail to read the shdr\n");
            assert(0);
        }
        if (shdr.sh_type == SHT_SYMTAB)
        {
            fseek(fd, shdr.sh_offset, SEEK_SET);
            // enter symtab
            Elf64_Sym sym;
            size_t sym_count = shdr.sh_size / shdr.sh_entsize;
            symbol = malloc(sizeof(Symbol) * sym_count);
            for (int i = 0; i < sym_count; i++)
            {
                if (fread(&sym, sizeof(Elf64_Sym), 1, fd) <= 0)
                {
                    printf("fail to read the symtab\n");
                    return;
                }
                if (ELF32_ST_TYPE(sym.st_info) == STT_FUNC) // if st_info ->func
                {
                    const char *name = string_table + sym.st_name;
                    strncpy(symbol[func_num].name, name, sizeof(symbol[func_num].name) - 1);
                    symbol[func_num].addr = sym.st_value;
                    symbol[func_num].size = sym.st_size;
                    func_num++;
                }
            }
        }
    }
    fclose(fd);
    free(string_table);
}