#include <isa.h>
#include <stdlib.h>
#include <time.h>
#include "local-include/reg.h"

const char *regsl[] = {"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"};
const char *regsw[] = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};
const char *regsb[] = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};

void reg_test()
{
  srand(time(0));
  word_t sample[8];
  word_t pc_sample = rand();
  cpu.pc = pc_sample;

  int i;
  for (i = R_EAX; i <= R_EDI; i++)
  {
    sample[i] = rand();
    reg_l(i) = sample[i];
    assert(reg_w(i) == (sample[i] & 0xffff));
  }

  assert(reg_b(R_AL) == (sample[R_EAX] & 0xff));
  assert(reg_b(R_AH) == ((sample[R_EAX] >> 8) & 0xff));
  assert(reg_b(R_BL) == (sample[R_EBX] & 0xff));
  assert(reg_b(R_BH) == ((sample[R_EBX] >> 8) & 0xff));
  assert(reg_b(R_CL) == (sample[R_ECX] & 0xff));
  assert(reg_b(R_CH) == ((sample[R_ECX] >> 8) & 0xff));
  assert(reg_b(R_DL) == (sample[R_EDX] & 0xff));
  assert(reg_b(R_DH) == ((sample[R_EDX] >> 8) & 0xff));

  assert(sample[R_EAX] == cpu.eax);
  assert(sample[R_ECX] == cpu.ecx);
  assert(sample[R_EDX] == cpu.edx);
  assert(sample[R_EBX] == cpu.ebx);
  assert(sample[R_ESP] == cpu.esp);
  assert(sample[R_EBP] == cpu.ebp);
  assert(sample[R_ESI] == cpu.esi);
  assert(sample[R_EDI] == cpu.edi);

  assert(pc_sample == cpu.pc);
}

void isa_reg_display()
{
  int length = sizeof(regsl) / sizeof(regsl[0]);
  for (int i = 0; i < length; i++)
  {
    uint32_t val = reg_l(i);
    printf("%s 0x%08x\t%20d\n", regsl[i], val, val);
  }
}

word_t isa_reg_str2val(const char *s, bool *success)
{
  printf("input ->%s\n", s);
  *success = true;

  if (!strcmp(s + 1, "eax"))
  {
    return cpu.eax;
  }
  else if (!strcmp(s + 1, "pc"))
  {
    return cpu.pc;
  }
  else if (!strcmp(s + 1, "ecx"))
  {
    return cpu.ecx;
  }
  else if (!strcmp(s + 1, "edx"))
  {
    return cpu.edx;
  }
  else if (!strcmp(s + 1, "ebx"))
  {
    return cpu.ebx;
  }
  else if (!strcmp(s + 1, "esp"))
  {
    return cpu.esp;
  }
  else if (!strcmp(s + 1, "ebp"))
  {
    return cpu.ebp;
  }
  else if (!strcmp(s + 1, "esi"))
  {
    return cpu.esi;
  }
  else if (!strcmp(s + 1, "edi"))
  {
    return cpu.edi;
  }
  else if (!strcmp(s + 1, "ax"))
  {
    return cpu.gpr[R_AX]._16;
  }
  else if (!strcmp(s + 1, "cx"))
  {
    return cpu.gpr[R_CX]._16;
  }
  else if (!strcmp(s + 1, "dx"))
  {
    return cpu.gpr[R_DX]._16;
  }
  else if (!strcmp(s + 1, "bx"))
  {
    return cpu.gpr[R_BX]._16;
  }
  else if (!strcmp(s + 1, "sp"))
  {
    return cpu.gpr[R_SP]._16;
  }
  else if (!strcmp(s + 1, "bp"))
  {
    return cpu.gpr[R_BP]._16;
  }
  else if (!strcmp(s + 1, "si"))
  {
    return cpu.gpr[R_SI]._16;
  }
  else if (!strcmp(s + 1, "di"))
  {
    return cpu.gpr[R_DI]._16;
  }
  else if (!strcmp(s + 1, "ah"))
  {
    return cpu.gpr[R_EAX]._8[1];
  }
  else if (!strcmp(s + 1, "ch"))
  {
    return cpu.gpr[R_ECX]._8[1];
  }
  else if (!strcmp(s + 1, "dh"))
  {
    return cpu.gpr[R_EDX]._8[1];
  }
  else if (!strcmp(s + 1, "bh"))
  {
    return cpu.gpr[R_EBX]._8[1];
  }
  else if (!strcmp(s + 1, "al"))
  {
    return cpu.gpr[R_EAX]._8[0];
  }
  else if (!strcmp(s + 1, "cl"))
  {
    return cpu.gpr[R_ECX]._8[0];
  }
  else if (!strcmp(s + 1, "dl"))
  {
    return cpu.gpr[R_EDX]._8[0];
  }
  else if (!strcmp(s + 1, "bl"))
  {
    return cpu.gpr[R_EBX]._8[0];
  }
  *success = false;
  return 0;
}