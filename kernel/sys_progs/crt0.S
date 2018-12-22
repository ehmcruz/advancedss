.globl _start, _exit
.extern main

.section .text

_start:
	movq $1, %rdi
	lea fname, %rsi
	push %rsi
	mov %rsp, %rsi
	callq main
	mov %rax, %rdi

_exit:
	mov %rdi, %rbx    /* save exit code */
	mov $60, %rax     /* 60 is exit syscall */
	syscall

.section .data

fname: .string "bin_name"
