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

    Elf64_Shdr shdr;                // section header
    long strtab_offset = -1;        // string table offset

    // 获取符号表的位置
    fseek(fp, ehdr.e_shoff, SEEK_SET);
    for (int i = 0; i < ehdr.e_shnum; i++) {
        int ret2 = fread(&shdr, sizeof(shdr), 1, fp);
        assert(ret2 == 1);

        if (shdr.sh_type == SHT_STRTAB) {
            strtab_offset = shdr.sh_offset;
            break;
        }
    }
    
    // read section header
    fseek(fp, ehdr.e_shoff, SEEK_SET);
    for (int i = 0; i < ehdr.e_shnum; i++) {
        int ret2 = fread(&shdr, sizeof(shdr), 1, fp);
        assert(ret2 == 1);

        if (shdr.sh_type == SHT_SYMTAB) {
            // read symbol table
            Elf64_Sym sym;
            fseek(fp, shdr.sh_offset, SEEK_SET);        // move to symbol table
            int i = 0;
            for (int j = 0; j < shdr.sh_size / sizeof(sym); j++) {      // read symbol table
                int ret3 = fread(&sym, sizeof(sym), 1, fp);


                // 打印symbol table
                printf("sym.st_name = %d, sym.st_info = %d, sym.st_other = %d, sym.st_shndx = %d, sym.st_value = %lx, sym.st_size = %ld\n",
                        sym.st_name, sym.st_info, sym.st_other, sym.st_shndx, sym.st_value, sym.st_size);

                if (ret3 != 1) {
                    // 打印调试信息
                    printf("j = %d, shdr.sh_size = %ld, sizeof(sym) = %ld, ret3 = %d\n", j, shdr.sh_size, sizeof(sym), ret3);
                }

                assert(ret3 == 1);
                if (ELF64_ST_TYPE(sym.st_info) == STT_FUNC) {  // ELF64_ST_TYPE的作用是取出符号类型
                    // 记录当前文件指针的位置
                    long cur_pos = ftell(fp);
                    // read function name
                    char *func_name = (char *)malloc(32);
                    // 从字符串表中读取对应函数名

                    fseek(fp, strtab_offset + sym.st_name, SEEK_SET);
                    int ret4 = fread(func_name, 32, 1, fp);

                    assert(ret4 == 1);
                    // read function address
                    uint32_t addr = sym.st_value;
                    // read function size
                    uint32_t size = sym.st_size;
                    // add to ftrace_info
                    func_info info = {addr, size, func_name};
                    func_table[i++] = info;
                    // 回复文件指针
                    fseek(fp, cur_pos, SEEK_SET);
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