#ifndef __MONITOR_MONITOR_H__
#define __MONITOR_MONITOR_H__

#include <common.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <elf.h>
typedef struct
{
  char name[64];
  paddr_t addr; // the function head address
  Elf64_Xword size;
} Symbol;
Symbol *symbol = NULL;
enum
{
  NEMU_STOP,
  NEMU_RUNNING,
  NEMU_END,
  NEMU_ABORT,
  NEMU_QUIT
};

typedef struct
{
  int state;
  vaddr_t halt_pc;
  uint32_t halt_ret;
} NEMUState;

extern NEMUState nemu_state;

void display_inv_msg(vaddr_t pc);

#endif
