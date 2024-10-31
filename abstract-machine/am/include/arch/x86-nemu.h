#ifndef ARCH_H__
#define ARCH_H__

struct Context
{
  uintptr_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
  int irq;
  uintptr_t error_code, eip, cs, eflags;
  void *cr3;
};

#define GPR1 eax
#define GPR2 eip
#define GPR3 eip
#define GPR4 eip
#define GPRx eip

#endif
