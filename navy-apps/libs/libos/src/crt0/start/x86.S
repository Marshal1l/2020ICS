.globl  _start
_start:
  movl $0, %ebp
  movl %eax, %edi
  pushl %esp
  call call_main
