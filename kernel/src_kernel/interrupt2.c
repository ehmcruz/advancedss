#include "interrupt.h"
#include "lib.h"
#include "config.h"
#include "printk.h"
#include "process.h"
#include "vars.h"

x86_84_int_vector_el interrupt_idt_tb[CONFIG_KERNEL_IDT_SIZE];

void idt_initialize()
{
	uint16_t i;
	KERNEL_ASSERT(sizeof(x86_84_int_vector_el) == 8);
	for (i=0; i<CONFIG_KERNEL_IDT_SIZE; i++) {
		interrupt_set_handler(i, interrupt_handler_null, 0);
	}

	interrupt_set_handler(6, interrupt_handler_invalid_opcode, 0);
	interrupt_set_handler(13, interrupt_handler_exception_gpf, 0);
	interrupt_set_handler(14, interrupt_handler_exception_page_fault, 0);
	interrupt_set_handler(32, interrupt_handler_timer, 0);
	interrupt_set_handler(33, interrupt_handler_external_hd, 0);
	interrupt_set_handler(34, interrupt_handler_keyboard, 0);
	
	write_msr_64(CONFIG_KERNEL_MSR_SYSCALL_HANDLER_ADDR, interrupt_handler_fast_syscall);
}

void interrupt_set_handler(uint8_t code, void *handler, uint8_t enable_software)
{
	interrupt_idt_tb[code].mask.handler_addr = (uint64_t)handler >> 2;
	interrupt_idt_tb[code].mask.permit_software = enable_software;
}

void interrupt_process_invalid_opcode()
{
	process_t *p;
	uint32_t cpuid;
	
	cpuid = get_cpuid();
	
	p = process_active_process_at_cpu[ cpuid ];
	p->state = PROCESS_STATE_EMPTY;
//	video_set_focus_window(0);
	interrupt_happened();
	printk_(p->video_buffer, "invalid opcode at rip 0x%llX\n", interrupt_context_saved_rip);
//	while (1);
	process_scheduler_schedule();
}

void interrupt_process_null_handler()
{
	process_t *p;
	uint32_t cpuid;
	
	cpuid = get_cpuid();
	
	p = process_active_process_at_cpu[ cpuid ];
	p->state = PROCESS_STATE_EMPTY;
//	video_set_focus_window();
	interrupt_happened();
	printk_(p->video_buffer, "an interrupt ocurred at rip 0x%llX\n", interrupt_context_saved_rip);
//	while (1);
	process_scheduler_schedule();
}

void interrupt_generel_protection_fault()
{
	process_t *p;
	uint32_t cpuid;
	
	cpuid = get_cpuid();
	
	p = process_active_process_at_cpu[ cpuid ];
	p->state = PROCESS_STATE_EMPTY;

	printk_(p->video_buffer, "general protection fault, trying to access 0x%llX at rip=0x%llX\n", get_virtual_address_fault(), interrupt_context_saved_rip);
	
	//video_set_focus_window(0);
	interrupt_happened();
	//halt();
	//while (1);
	process_scheduler_schedule();
}

void interrupt_page_fault()
{
	process_t *p;
	thread_t *t;
	uint32_t cpuid;
	
//video_set_focus_window(0);
	interrupt_happened();
	
	cpuid = get_cpuid();
	
	p = process_active_process_at_cpu[ cpuid ];
	t = process_active_thread_at_cpu[ cpuid ];
	
	p->state = PROCESS_STATE_EMPTY;
	
	printk_(p->video_buffer, "page fault, trying to access 0x%llX at rip=0x%llX\n", get_virtual_address_fault(), interrupt_context_saved_rip);
	printk_(p->video_buffer, "stack start=0x%llX, stack_end=0x%llX\nmheap_end=0x%llX, static heap_end=0x%llX\n", p->stack_start, p->stack_end, p->heap_end, p->static_heap_end);
//halt();
//	while (1);
	process_scheduler_schedule();
}

void interrupt_happened()
{
	thread_t *t;
	uint32_t cpuid;
	
	cpuid = get_cpuid();
	
	t = process_active_thread_at_cpu[ cpuid ];
	t->state = THREAD_STATE_READY;

	process_thread_save_context(process_active_process_at_cpu[ cpuid ], process_active_thread_at_cpu[ cpuid ]);

//printk_(t->owner->video_buffer, "interrupt happennded rip=0x%llX", t->rip);	
}
