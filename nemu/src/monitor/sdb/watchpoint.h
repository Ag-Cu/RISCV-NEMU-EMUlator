#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include <common.h>

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
  char *args;
  word_t val;
} WP;


WP* new_wp();
void free_wp(int no);
bool check_wp();
void watchpoint_display();
#endif