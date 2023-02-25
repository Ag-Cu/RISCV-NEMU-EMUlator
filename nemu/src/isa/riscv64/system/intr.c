/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <isa.h>

extern riscv64_CSR_state CSRs;

char *isa_intr_name(uint32_t NO) {
  static char *str[] = { "Reserved", 
                         "Supervisor software interrupt", 
                         "Virtual supervisor software interrupt",
                          "Machine software interrupt",
                          "Reserved",
                          "Supervisor timer interrupt",
                          "Virtual supervisor timer interrupt",
                          "Machine timer interrupt",
                          "Reserved",
                          "Supervisor external interrupt",
                          "Virtual supervisor external interrupt",
                          "Machine External Interrupt",
                          "Supervisor guest external interrupt",
                          "Reserved",
                          "Reserved",
                           };
  if (NO >= sizeof(str) / sizeof(str[0])) {
    return "Designated for platform or custom use";
  }
  return str[NO];
}

word_t isa_raise_intr(word_t NO, vaddr_t epc) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * Then return the address of the interrupt/exception vector.
   */
  CSRs.mstatus = 0xa00001800;
  CSRs.mepc = epc;
  CSRs.mcause = NO;
  vaddr_t mtvec = CSRs.mtvec;

  // exception trace
  #ifdef CONFIG_ETRACE_COND
    printf("Exception: %s\n", isa_intr_name(NO));
  #endif
  return mtvec;
}

word_t isa_query_intr() {
  return INTR_EMPTY;
}
