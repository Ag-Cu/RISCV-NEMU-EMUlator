#include "ftracer.h"



char *get_func_name(uint32_t addr) {
    for (int i = 0; i < func_num; ++i) {
        uint32_t start_addr = func_table[i].start_addr;
        int size = func_table[i].size;
        if (addr >= start_addr && addr < start_addr + size) {
            return func_table[i].func_name;
        } 
    }
    return NULL;
}

void print_ftrace_info(){
    int blank_num = 0;
    for (int i = 0; i < ftrace_index; ++i) {
        for (int i = 0; i < blank_num; i++) {
            printf(" ");
        }
        char *type = call_ret_table[i].type == 0 ? "call" : "ret";
        printf("%s [@%s 0x%x]\n", type,call_ret_table[i].func_name, call_ret_table[i].addr);
        if (call_ret_table[i].type == 0) {
            ++blank_num;
        } else {
            --blank_num;
        }
    }
}


void init_ftracer(char *elf_file){
    assert(elf_file != NULL);
    /* init elf_loader */
    init_elf(elf_file);
}