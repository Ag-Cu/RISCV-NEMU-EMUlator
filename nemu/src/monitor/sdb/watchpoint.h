#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include <common.h>

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
  bool status;
  char *args;
  word_t val;
} WP;


WP* new_wp();
void free_wp(WP *wp);
bool check_wp();

#endif