#ifndef _KERNEL_HEADER_INTERRUPT_H_
#define _KERNEL_HEADER_INTERRUPT_H_

#include "config.h"
#include "lib.h"

void idt_initialize();
void interrupt_set_handler(uint8_t code, void *handler, uint8_t enable_software);

void interrupt_handler_external_hd();
void interrupt_handler_keyboard();
void interrupt_handler_null();
void interrupt_handler_timer();
void interrupt_handler_exception_gpf();
void interrupt_handler_exception_page_fault();
void interrupt_handler_fast_syscall();
void interrupt_handler_invalid_opcode();

union x86_84_int_vector_el {
	struct {
		uint64_t handler_addr: 62; // handler address is aligned to 4 bytes
		uint64_t permit_software: 1; // allow for software interrupt
	} mask;

	uint64_t data;
};

typedef union x86_84_int_vector_el x86_84_int_vector_el;

#endif
