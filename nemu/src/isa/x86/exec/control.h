#include "cc.h"

const char *check_func_call(paddr_t insta_addr, int iscall);
static inline def_EHelper(jmp)
{
  // the target address is calculated at the decode stage
  rtl_j(s, s->jmp_pc);

  print_asm("jmp %x", s->jmp_pc);
}

static inline def_EHelper(jcc)
{
  // the target address is calculated at the decode stage
  uint32_t cc = s->opcode & 0xf;
  rtl_setcc(s, s0, cc);
  rtl_jrelop(s, RELOP_NE, s0, rz, s->jmp_pc);

  print_asm("j%s %x", get_cc_name(cc), s->jmp_pc);
}

static inline def_EHelper(jmp_rm)
{
  rtl_jr(s, ddest);

  print_asm("jmp *%s", id_dest->str);
}

static inline def_EHelper(call)
{
  // the target address is calculated at the decode stage

  rtl_push(s, &s->seq_pc);
  rtl_jr(s, &s->jmp_pc);
  const char *tmp_cr = check_func_call(s->jmp_pc, 1);
  if (!strcmp(tmp_cr, "???"))
  {
    assert(0);
  }
  //++func_deepth;
  add_call_ret("call %s<%x>", tmp_cr, s->jmp_pc);
  print_asm("call %s<%x>", tmp_cr, s->jmp_pc);
}

static inline def_EHelper(ret)
{
  // TODO();
  if (s->isa.is_operand_size_16 == true)
  {
    rtl_pop(s, &s->seq_pc);
    s->seq_pc &= 0x0000FFFF;
  }
  else
  {
    rtl_pop(s, &s->seq_pc);
  }
  //--func_deepth;
  const char *tmp_cr = check_func_call(s->seq_pc, 0);
  if (!strcmp(tmp_cr, "???"))
  {
    assert(0);
  }
  add_call_ret("ret %s<%x>", tmp_cr, s->seq_pc);
  print_asm("ret %s<%x>", tmp_cr, s->seq_pc);
}

static inline def_EHelper(ret_imm)
{
  TODO();
  print_asm("ret %s", id_dest->str);
}

static inline def_EHelper(call_rm)
{
  // TODO();
  rtl_push(s, &s->seq_pc);
  rtl_jr(s, ddest);

  const char *tmp_cr = check_func_call(*ddest, 1);
  if (!strcmp(tmp_cr, "???"))
  {
    assert(0);
  }
  add_call_ret("call *%s<%x>", *ddest, tmp_cr);
  // print_asm("call *%s<%x>", id_dest->str, tmp_cr);
}
