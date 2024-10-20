#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include <common.h>
#define NR_WP 10
#define WPEXPRLEN 30
typedef struct watchpoint
{
  int NO;
  struct watchpoint *next;
  char wp_expr[WPEXPRLEN];
  /* TODO: Add more members if necessary */
  uint32_t old_val;
} WP;
word_t expr(char *e, bool *success);
WP *new_wp(char *s);
void free_wp(int no_delete);
int char2int(char s[]);
void info_w_display();
void info_fw_display();
void check_wp();

#endif
