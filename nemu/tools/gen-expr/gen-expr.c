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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

uint32_t buf_index;

uint32_t choose(uint32_t n) {
  return rand() % n;
}

void gen_space() {
  if (choose(3) == 0) {
    int i = choose(3);
    while (i--) {
      buf[buf_index++] = ' ';
    }
  }
}

void gen(char c) {
  gen_space();
  buf[buf_index++] = c;
  gen_space();
}

void gen_num() {
  gen_space();
  int num = choose(1000);
  char str[10];
  sprintf(str, "%d", num);
  for (int i = 0; i < sizeof(str) - 1; i++) {
    buf[buf_index++] = str[i];
  }
  gen_space();
}

void gen_rand_op() {
  gen_space();
  switch (choose(4))
  {
  case 0:
    buf[buf_index++] = '+';
    break;
  case 1:
    buf[buf_index++] = '-';
    break;
  case 2:
    buf[buf_index++] = '*';
    break;
  default:
    buf[buf_index++] = '/';
    break;
  }
  gen_space();
}

static void gen_rand_expr() {
  gen_space();
  switch (choose(3))
  {
  case 0:
    gen_num();
    break;
  case 1:
    gen('(');
    gen_rand_expr();
    gen(')');
    break;
  default:
    gen_rand_expr();
    gen_rand_op();
    gen_rand_expr();
    break;
  }
  gen_space();
}

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    buf_index = 0;
    gen_rand_expr();
    buf[buf_index] = '\0';
    sprintf(code_buf, code_format, buf);        // buf -> code_buf

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);                        // code_buf -> /tmp/.code.c
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
