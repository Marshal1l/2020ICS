
#include "ftrace.h"
#include "debug.h"
Symbol *symbol = NULL;
static int func_num = 0;
const char *check_func_call(paddr_t insta_addr)
{
    if (symbol == NULL)
        return "NULL";
    Symbol *tmpsymbol = symbol;
    while (tmpsymbol != NULL)
    {
        if (insta_addr >= tmpsymbol->addr && insta_addr < tmpsymbol->addr + tmpsymbol->size)
        {
            return (const char *)tmpsymbol->name;
        }
        tmpsymbol++;
    }
    return "???";
}
void init_ftrace(const char *elf_file)
{
    Log("init ftace\n");
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
    Elf32_Ehdr ehdr;
    if (fread(&ehdr, sizeof(Elf32_Ehdr), 1, fd) <= 0)
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
    Elf32_Shdr shdr;
    char *string_table = NULL;
    for (int i = 0; i < ehdr.e_shnum; i++)
    {
        if (fread(&shdr, sizeof(Elf32_Shdr), 1, fd) <= 0)
        {
            printf("fail to read the shdr\n");
            assert(0);
        }

        if (shdr.sh_type == SHT_STRTAB)
        {
            // printf("shdr_sh_size:%x\n", shdr.sh_size);
            // pause();
            // get string table
            string_table = (char *)malloc(shdr.sh_size);
            fseek(fd, shdr.sh_offset, SEEK_SET);
            if (fread(string_table, shdr.sh_size, 1, fd) <= 0)
            {
                printf("fail to read the strtab\n");
                exit(0);
            }
        }
    }

    for (int i = 0; i < shdr.sh_size / sizeof(char); i++)
    {
        printf("string table:%c\n", string_table[0]);
    }

    fseek(fd, ehdr.e_shoff, SEEK_SET);
    for (int i = 0; i < ehdr.e_shnum; i++)
    {
        if (fread(&shdr, sizeof(Elf32_Shdr), 1, fd) <= 0)
        {
            printf("fail to read the shdr\n");
            assert(0);
        }
        if (shdr.sh_type == SHT_SYMTAB)
        {
            fseek(fd, shdr.sh_offset, SEEK_SET);
            // enter symtab
            Elf32_Sym sym;
            size_t sym_count = shdr.sh_size / shdr.sh_entsize;
            symbol = malloc(sizeof(Symbol) * sym_count);
            for (int i = 0; i < sym_count; i++)
            {
                if (fread(&sym, sizeof(Elf32_Sym), 1, fd) <= 0)
                {
                    printf("fail to read the symtab\n");
                    return;
                }
                if (ELF32_ST_TYPE(sym.st_info) == STT_FUNC) // if st_info ->func
                {
                    printf("find func!\n");
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
    Symbol *tmpsymbol = symbol;
    while (tmpsymbol != NULL)
    {
        printf("symbol:%s\n", symbol->name);
        tmpsymbol++;
    }
}