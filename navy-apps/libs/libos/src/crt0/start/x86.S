.globl  _start
_start:
  movl $0, %ebp
  movl %eax, %esp
  pushl $0
  call call_main
