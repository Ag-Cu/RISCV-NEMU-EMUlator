#ifndef __FTRACER_H__
#define __FTRACER_H__

#include <common.h>
#include "elf_loader.h"

void init_ftracer(char *elf_file);

char *get_func_name(uint32_t addr, int *type);

void print_ftrace_info();



#endif
