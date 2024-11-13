.globl  _start
_start:
  movl $0, %ebp
  movl %eax, %esp
  pushl $1
  call call_main
