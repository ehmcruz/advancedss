#include "config.h"

.globl interrupt_context_saved_gpr, interrupt_context_saved_rip, interrupt_context_saved_rflags, lock_kernel, unlock_kernel, interrupt_handler_keyboard, interrupt_handler_external_hd, interrupt_handler_null, interrupt_handler_timer, interrupt_handler_exception_gpf, interrupt_handler_fast_syscall, interrupt_handler_exception_page_fault, interrupt_handler_invalid_opcode
.extern interrupt_process_null_handler, process_scheduler_timer_interrupt, interrupt_generel_protection_fault, external_hd_interrupt, keyboard_interrupt, process_scheduler_schedule, interrupt_happened

.section .data
/*
	used to let only one processor inside the kernel
*/
kernel_locked: .word 0

interrupt_context_saved_gpr: .skip CONFIG_KERNEL_NUMBER_GPR * 8  /* gpr context save */
interrupt_context_saved_rip: .quad 0 /* rip context save */
interrupt_context_saved_rflags: .quad 0 /* rflags context save */

.section .text

lock_kernel:
1:
	lock btsw $0, kernel_locked
	jb 1b
	retq

unlock_kernel:
	movb $0, kernel_locked
	retq

.macro save_gpr gpr, code
	movq \gpr, interrupt_context_saved_gpr+(8*\code)
.endm

.macro load_gpr gpr, code
	movq interrupt_context_saved_gpr+(8*\code), \gpr
.endm

.macro save_context
	save_gpr %rax, 0
	save_gpr %rcx, 1
	save_gpr %rdx, 2
	save_gpr %rbx, 3
//	save_gpr %rsp, 4
	save_gpr %rbp, 5
	save_gpr %rsi, 6
	save_gpr %rdi, 7
	save_gpr %r8, 8
	save_gpr %r9, 9
	save_gpr %r10, 10
	save_gpr %r11, 11
	save_gpr %r12, 12
	save_gpr %r13, 13
	save_gpr %r14, 14
	save_gpr %r15, 15
	
	movq 16(%rsp), %r15
	movq %r15, interrupt_context_saved_rflags
	movq 8(%rsp), %r15
	movq %r15, interrupt_context_saved_gpr+(8*4)
	movq (%rsp), %r15
	movq %r15, interrupt_context_saved_rip
.endm

.macro load_context
	load_gpr %rax, 0
	load_gpr %rcx, 1
	load_gpr %rdx, 2
	load_gpr %rbx, 3
//	load_gpr %rsp, 4
	load_gpr %rbp, 5
	load_gpr %rsi, 6
	load_gpr %rdi, 7
	load_gpr %r8, 8
	load_gpr %r9, 9
	load_gpr %r10, 10
	load_gpr %r11, 11
	load_gpr %r12, 12
	load_gpr %r13, 13
	load_gpr %r14, 14

	movq interrupt_context_saved_rflags, %r15
	movq %r15, 16(%rsp)
	movq interrupt_context_saved_gpr+(8*4), %r15
	movq %r15, 8(%rsp)
	movq interrupt_context_saved_rip, %r15
	movq %r15, (%rsp)

	load_gpr %r15, 15
.endm

/* remember that all interrupt handlers must be aligned to 4 bytes */

.balign 4
interrupt_handler_timer:
	callq lock_kernel
	save_context
/*	callq interrupt_happened*/
	callq process_scheduler_timer_interrupt
	load_context
	callq unlock_kernel
	iret

.balign 4
interrupt_handler_exception_gpf:
	callq lock_kernel
	save_context
	/*callq interrupt_happened*/
	callq interrupt_generel_protection_fault
	/*callq process_scheduler_schedule*/
	load_context
	callq unlock_kernel
	iret

.balign 4
interrupt_handler_exception_page_fault:
	callq lock_kernel
	save_context
	/*callq interrupt_happened*/
	callq interrupt_page_fault
	/*callq process_scheduler_schedule*/
	load_context
	callq unlock_kernel
	iret

.balign 4
interrupt_handler_keyboard:
	callq lock_kernel
	save_context
	/*callq interrupt_happened*/
	callq keyboard_interrupt
	/*callq process_scheduler_schedule*/
	load_context
	callq unlock_kernel
	iret

.balign 4
interrupt_handler_external_hd:
	callq lock_kernel
	save_context
	/*callq interrupt_happened*/
	callq external_hd_interrupt
	/*callq process_scheduler_schedule*/
	load_context
	callq unlock_kernel
	iret

.balign 4
interrupt_handler_null:
	callq lock_kernel
	save_context
	/*callq interrupt_happened*/
	callq interrupt_process_null_handler
/*	callq process_scheduler_schedule*/
	load_context
	callq unlock_kernel
	iret

.balign 4
interrupt_handler_invalid_opcode:
	callq lock_kernel
	save_context
	/*callq interrupt_happened*/
	callq interrupt_process_invalid_opcode
/*	callq process_scheduler_schedule*/
	load_context
	callq unlock_kernel
	iret

/******************************************/

/* system calls, implemented throw syscall instruction, like linux */

interrupt_handler_fast_syscall:
	callq lock_kernel
	
	/* save context */

	save_gpr %rax, 0
	save_gpr %rcx, 1
	save_gpr %rdx, 2
	save_gpr %rbx, 3
//	save_gpr %rsp, 4
	save_gpr %rbp, 5
	save_gpr %rsi, 6
	save_gpr %rdi, 7
	save_gpr %r8, 8
	save_gpr %r9, 9
	save_gpr %r10, 10
	save_gpr %r11, 11
	save_gpr %r12, 12
	save_gpr %r13, 13
	save_gpr %r14, 14
	save_gpr %r15, 15
	
	movq %r11, interrupt_context_saved_rflags
	movq %rcx, interrupt_context_saved_rip
	movq (%rsp), %r15
	movq %r15, interrupt_context_saved_gpr+(8*4)
	
	movq %r11, (%rsp)
	subq $8, %rsp
	movq %r15, (%rsp)
	subq $8, %rsp
	movq %rcx, (%rsp)

	/* context saved */
	
	lea syscall_table, %r15
	lea (%r15 , %rax, 8), %r15
	cmp $syscall_table_end, %r15
	jge 1f
	mov (%r15), %r15
	cmp $0, %r15
	je 1f
	callq *%r15
	jmp 2f
1:
	movq %rax, %rdi
	callq syscall_invalid
2:
	callq process_scheduler_schedule
	load_context
	callq unlock_kernel
	iret

fast_syscall_read:
	callq syscall_read
	retq

fast_syscall_write:
	callq syscall_write
	retq

fast_syscall_open:
	callq syscall_open
	retq

fast_syscall_brk:
	callq syscall_brk
	retq

fast_syscall_close:
	callq syscall_close
	retq

fast_syscall_fork:
	callq syscall_fork
	retq

fast_syscall_getpid:
	callq syscall_getpid
	retq

fast_syscall_writev:
	callq syscall_writev
	retq

fast_syscall_fstat:
	callq syscall_fstat
	retq

fast_syscall_uname:
	callq syscall_uname
	retq

.section .data

.balign 8
syscall_table:
	.quad fast_syscall_read
	.quad fast_syscall_write
	.quad fast_syscall_open
	.quad fast_syscall_close
	.quad 0
	.quad 0
	.quad 0
	.quad 0
	.quad 0
	.quad 0
	.quad 0
	.quad 0
	.quad fast_syscall_brk
	.quad 0
	.quad 0
	.quad 0
	.quad 0
	.quad 0
	.quad 0
	.quad 0
	.quad fast_syscall_writev
	.quad 0
	.quad 0
	.quad 0
	.quad 0
	.quad 0
	.quad 0
	.quad 0
	.quad 0
	.quad 0
	.quad 0
	.quad 0
	.quad 0
	.quad 0
	.quad 0
	.quad 0
	.quad 0
	.quad 0
	.quad 0
	.quad fast_syscall_getpid
	.quad 0
	.quad 0
	.quad 0
	.quad 0
	.quad 0
	.quad 0
	.quad 0
	.quad 0
	.quad 0
	.quad 0
	.quad 0
	.quad 0
	.quad 0
	.quad 0
	.quad 0
	.quad 0
	.quad 0
	.quad fast_syscall_fork
	.quad 0
	.quad 0
	.quad 0
	.quad 0
	.quad 0
	.quad fast_syscall_uname
	.quad 0
	.quad 0
	.quad 0
	.quad 0
	.quad 0
	.quad 0
	.quad 0
	.quad 0

syscall_table_end:
	.quad 0
