#include "elf_loader.h"
#include <elf.h>

func_info func_table[1024] = {0};
int func_num = 0;
ftrace_info call_ret_table[1024] = {0};
int ftrace_index = 0;


FILE *read_file(const char *elf_file) {
    Log("Loading from %s", elf_file);
    FILE *fp = fopen(elf_file, "rb");
    Assert(fp, "Can not open '%s'", elf_file);
    return fp;
}




void init_elf(const char *elf_file) {
    // TODO();
    // Log("Initializing elf tables...");
    FILE *fp = read_file(elf_file);
    Elf64_Ehdr ehdr;                // elf header
    int ret1 = fread(&ehdr, sizeof(ehdr), 1, fp);
    assert(ret1 == 1);

    // 打印elf header
/*  printf("e_ident: %s, e_type: %d, e_machine: %d, e_version: %d, e_entry: %lx, e_phoff: %ld, e_shoff: %ld, e_flags: %d, e_ehsize: %d, e_phentsize: %d, e_phnum: %d, e_shentsize: %d, e_shnum: %d, e_shstrndx: %d\n",
            ehdr.e_ident, ehdr.e_type, ehdr.e_machine, ehdr.e_version, ehdr.e_entry, ehdr.e_phoff, ehdr.e_shoff, ehdr.e_flags, ehdr.e_ehsize, ehdr.e_phentsize, ehdr.e_phnum, ehdr.e_shentsize, ehdr.e_shnum, ehdr.e_shstrndx);
*/
    Elf64_Shdr shdr;                // section header
    fseek(fp, ehdr.e_shoff, SEEK_SET);
    
    // read section header
    for (int i = 0; i < ehdr.e_shnum; i++) {
        int ret2 = fread(&shdr, sizeof(shdr), 1, fp);
        assert(ret2 == 1);
        // 打印section header
/*     printf("sh_name: %d, sh_type: %d, sh_flags: %ld, sh_addr: %ld, sh_offset: %ld, sh_size: %ld, sh_link: %d, sh_info: %d, sh_addralign: %ld, sh_entsize: %ld\n",
                shdr.sh_name, shdr.sh_type, shdr.sh_flags, shdr.sh_addr, shdr.sh_offset, shdr.sh_size, shdr.sh_link, shdr.sh_info, shdr.sh_addralign, shdr.sh_entsize);
*/      if (shdr.sh_type == SHT_SYMTAB) {
            // read symbol table
            Elf64_Sym sym;
            fseek(fp, shdr.sh_offset, SEEK_SET);        // move to symbol table
            int i = 0;
            for (int j = 0; j < shdr.sh_size / sizeof(sym); j++) {      // read symbol table
                int ret3 = fread(&sym, sizeof(sym), 1, fp);


                if (ret3 != 1) {
                    // 打印调试信息
                    printf("j = %d, shdr.sh_size = %ld, sizeof(sym) = %ld, ret3 = %d\n", j, shdr.sh_size, sizeof(sym), ret3);
                }
                if (j == 23) {
                    printf("sym.st_name = %d, sym.st_info = %d, sym.st_other = %d, sym.st_shndx = %d, sym.st_value = %ld, sym.st_size = %ld\n",
                            sym.st_name, sym.st_info, sym.st_other, sym.st_shndx, sym.st_value, sym.st_size);
                }

                assert(ret3 == 1);
                if (ELF64_ST_TYPE(sym.st_info) == STT_FUNC) {  // ELF64_ST_TYPE的作用是取出符号类型
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
                    // free(func_name);
                }
            }
            func_num = i;
        }
        // Log("Initializing elf tables...done");
    }

    // print func_table
    Log("func_num = %d", func_num);
    for (int i = 0; i < func_num; i++) {
        printf("func_table[%d]: addr = 0x%x, size = 0x%x, name = %s\n",
                 i, func_table[i].start_addr, func_table[i].size, func_table[i].func_name);
    }
}