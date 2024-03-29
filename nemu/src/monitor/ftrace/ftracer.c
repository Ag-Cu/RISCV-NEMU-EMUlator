#include "ftracer.h"



char *get_func_name(uint32_t cur_addr, uint32_t des_addr, int *type) {
    for (int i = 0; i < func_num; ++i) {
        uint32_t start_addr = func_table[i].start_addr;
        if (des_addr == start_addr) {
            *type = 0;       // call
            return func_table[i].func_name;
        }
    }
    for (int i = 0; i < func_num; ++i) {
        uint32_t start_addr = func_table[i].start_addr;
        uint32_t size = func_table[i].size;
        if (cur_addr >= start_addr && cur_addr < start_addr + size) {
            *type = 1;       // ret
            return func_table[i].func_name;
        }
    }
    return NULL;
}

void print_ftrace_info(){
    int blank_num = -1;
    for (int i = 0; i < ftrace_index; ++i) {

        if (call_ret_table[i].type == 0) {
            ++blank_num;
        }
        printf("0x%x: ", call_ret_table[i].ins_addr);
        for (int i = 0; i < blank_num; i++) {
            printf(" ");
        }
        char *type = call_ret_table[i].type == 0 ? "call" : "ret";
        printf("%s [@%s 0x%x]\n", type,call_ret_table[i].func_name, call_ret_table[i].des_addr);

        if (call_ret_table[i].type == 1) {
            --blank_num;
        }
    }
}


void init_ftracer(char *elf_file){
    assert(elf_file != NULL);
    /* init elf_loader */
    init_elf(elf_file);
}