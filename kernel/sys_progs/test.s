.globl main

.section .text

main:
	mov $4, %rax
	mov $1, %rbx
	lea aaa, %rcx
	mov $4, %rdx
	int $0x80
	mov $1, %rax
	mov $4, %rdx
	lea aaa, %rsi
	mov $1, %rdi
	syscall
	retq
	
.section .data

aaa: .string "ola\n"
