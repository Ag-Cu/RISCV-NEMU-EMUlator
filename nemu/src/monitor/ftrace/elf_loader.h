#ifndef __ELF_LOADER_H__
#define __ELF_LOADER_H__

#include <common.h>


void init_elf(const char *elf_file);

bool examine_elf(uint32_t addr);

typedef struct {
    uint32_t start_addr;    // start address of the function
    uint32_t size;          // size of the function
    char *func_name;        // name of the function
} func_info;

typedef struct {
    char *func_name;        
    uint32_t addr;
    uint8_t type;       // 0 is call, 1 is ret
    uint32_t ins_addr;  // the address of the instruction
} ftrace_info;

extern func_info func_table[1024];

extern int func_num;

extern ftrace_info call_ret_table[1024];

extern int ftrace_index;

#endif