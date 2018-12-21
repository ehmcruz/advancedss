#include "config.h"

.globl _start, kernel_binary_image_size, kernel_physical_memory_size, kernel_stack_base_addr, kernel_stack_base_last_addr, kernel_physical_memory_usable_size
.extern lock_kernel, unlock_kernel, setup_cpu, start_c, _end

.section .data

kernel_binary_image_size: .long 0
kernel_physical_memory_size: .quad 0
kernel_physical_memory_usable_size: .quad 0

/*
	each cpu has its own stack for kernel mode
*/

kernel_stack_base_addr: .skip 8 * CONFIG_KERNEL_MAX_CPUS

kernel_stack_base_last_addr: .quad 0 /* address of the end of kernel stack space */

kernel_initialized: .byte 0

.section .text

/*
	initial state of registers:

	%rax: kernel length

	total number of processors can be obtained with msr(0) * msr(1)
*/

_start:
	lea _end, %r14
	mov %r14, %rax

	/* verify if kernel supports the number of processors specified */

	mov $CONFIG_KERNEL_MSR_NUMBER_PHY_CPUS, %ecx
	rdmsr
	mov %eax, %edi
	xor %eax, %eax
	mov $CONFIG_KERNEL_MSR_NUMBER_VIRTUAL_CPUS_PER_PHY_CPU, %ecx
	rdmsr
	mul %edi
	cmp $CONFIG_KERNEL_MAX_CPUS, %eax
	jle 1f

	/* unsupported number of cpus */
	hlt

1:
	/* setup stack for current cpu */

	mov $CONFIG_KERNEL_MSR_CPUID, %ecx
	rdmsr /* %eax now has the current processor id */
	mov %rax, %r8 /* %r8 now has the current processor id */
	mov $CONFIG_KERNEL_STACK_SIZE, %rcx
	add $1, %rax
	mul %rcx
	add %r14, %rax
	
	/* now %rax has the stack address for the current processor */

	mov %rax, %rbp
	mov %rax, %rsp

	sub $24, %rsp /* allocate space for first program iret. */

	/* set the kernel stack address in cpu msr */

	mov %rbp, %rdx
	shr $32, %rdx
	mov $CONFIG_KERNEL_MSR_KERNEL_STACK, %ecx
	wrmsr

	/* now save this stack address so we can use it later */

	lea kernel_stack_base_addr, %rcx
	lea (%rcx, %r8, 8), %rax
	mov %rbp, (%rax)

	/*
		cpu stack configured

		now we can make function calls
	*/

	/* let only one cpu to go */
	callq lock_kernel

	testb $1, kernel_initialized
	jnz 1f  /* jump if kernel was already initialized by another cpu */

	movb $1, kernel_initialized

	/* save kernel vars */

	movl %r14d, kernel_binary_image_size

	callq start_c

1:
	callq setup_cpu
	callq unlock_kernel
	iret

