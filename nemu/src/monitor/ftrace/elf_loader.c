#include "elf_loader.h"
#include <elf.h>

func_info func_table[1024] = {0};
int func_num = 0;
ftrace_info call_ret_table[1024] = {0};
int ftrace_index = 0;


FILE *read_file(const char *elf_file) {
    FILE *fp = fopen(elf_file, "rb");
    Assert(fp, "Can not open '%s'", elf_file);
    return fp;
}




void init_elf(const char *elf_file) {
    // TODO();
    // Log("Initializing elf tables...");
    // Log("Initializing elf tables...done");
    FILE *fp = read_file(elf_file);
    Elf32_Ehdr ehdr;                // elf header
    int ret1 = fread(&ehdr, sizeof(ehdr), 1, fp);
    assert(ret1 == 1);

    Elf32_Shdr shdr;                // section header
    fseek(fp, ehdr.e_shoff, SEEK_SET);
    
    // read section header
    for (int i = 0; i < ehdr.e_shnum; i++) {
        int ret2 = fread(&shdr, sizeof(shdr), 1, fp);
        assert(ret2 == 1);
        if (shdr.sh_type == SHT_SYMTAB) {
            // read symbol table
            Elf32_Sym sym;
            fseek(fp, shdr.sh_offset, SEEK_SET);        // move to symbol table
            int i = 0;
            for (int j = 0; j < shdr.sh_size / sizeof(sym); j++) {      // read symbol table
                int ret3 = fread(&sym, sizeof(sym), 1, fp);
                assert(ret3 == 1);
                if (ELF32_ST_TYPE(sym.st_info) == STT_FUNC) {  // ELF32_ST_TYPE的作用是取出符号类型
                    // read function name
                    char *func_name = (char *)malloc(32);
                    fseek(fp, shdr.sh_link * sizeof(shdr) + ehdr.e_shoff + sym.st_name, SEEK_SET);      // move to function name
                    int ret4 = fread(func_name, 32, 1, fp);
                    assert(ret4 == 1);
                    // read function address
                    uint32_t addr = sym.st_value;
                    // read function size
                    uint32_t size = sym.st_size;
                    // add to ftrace_info
                    func_info info = {addr, size, func_name};
                    func_table[i++] = info;
                    free(func_name);
                }
            }
            func_num = i;
        }
    }
}