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
} ftrace_info;

func_info func_table[1024] = {0};

int func_num = 0;

ftrace_info call_ret_table[1024] = {0};

int ftrace_index = 0;

#endif