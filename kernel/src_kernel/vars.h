#ifndef _KERNEL_HEADER_VARS_H_
#define _KERNEL_HEADER_VARS_H_

#include "config.h"
#include "process.h"
#include "video.h"
#include "interrupt.h"

// defined in entry.S
extern uint32_t kernel_binary_image_size;
extern uint64_t kernel_physical_memory_size;
extern uint64_t kernel_physical_memory_usable_size;
extern uint64_t kernel_memory;
extern void **kernel_stack_base_addr;
extern void *kernel_stack_base_last_addr;

// defined in interrupt.S
extern uint64_t interrupt_context_saved_gpr[CONFIG_KERNEL_NUMBER_GPR];
extern uint64_t interrupt_context_saved_rip;
extern uint64_t interrupt_context_saved_rflags;

// defined in interrupt2.c
extern x86_84_int_vector_el interrupt_idt_tb[CONFIG_KERNEL_IDT_SIZE];
// defined in process.c
#ifndef _KERNEL_IMPLEMENTATION_PROCESS_C_
	extern process_t *process_table;
	extern process_t *process_idle_process[CONFIG_KERNEL_MAX_CPUS];
	extern thread_t *process_idle_thread[CONFIG_KERNEL_MAX_CPUS];
	extern process_t *process_active_process_at_cpu[CONFIG_KERNEL_MAX_CPUS];
	extern thread_t *process_active_thread_at_cpu[CONFIG_KERNEL_MAX_CPUS];
	extern process_t *process_blocked_window_keyboard[CONFIG_KERNEL_NUMBER_OF_WINDOWS];
#endif

// defined in video.c
//extern video_data_t video_frame_buffer_windows[ CONFIG_KERNEL_NUMBER_OF_WINDOWS ];

#endif
