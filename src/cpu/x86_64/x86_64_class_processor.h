/*
    ADVANCEDSS (Advanced Superscalar Simulator).

    Copyright (C) 2009  Eduardo Henrique Molina da Cruz

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.


    **************************************************************************************

    ADVANCEDSS (Advanced Superscalar Simulator)

    Coded by Eduardo Henrique Molina da Cruz
    email: eduardohmdacruz@gmail.com
    MSN: eduardohmcruz@hotmail.com

    Oriented by Ronaldo Augusto de Lara Gonçalves
    Doctor in Computer Science
    State University of Maringa
    Parana - Brazil

    January 2009
*/

#ifndef _ADVANCEDSS_X8664_CLASS_PROCESSOR_HEADER_
#define _ADVANCEDSS_X8664_CLASS_PROCESSOR_HEADER_

#include "bochs/cpu_class.h"

protected:

	uint64_t model_specific_regs[CPU_X8664_NUMBER_MSR];

	x86_64_virtual_memory_cfg *msr_virtual_memory_mask;
	x86_64_virtual_memory_data2 virtual_memory_translate;

	uint8_t cpl: 2; // current priviledge level
	uint8_t interrupt_enabled: 1;
	
	struct x86_64_virtual_memory_pg_el_cache {
		x86_64_virtual_memory_data el;
		uint64_t vaddr;
		uint8_t busy;
	};

public:

	x86_64_virtual_memory_pg_el_cache x86_64_vm_cache[CPU_X86_64_VIRTUAL_MEMORY_CACHE_TABLE_SIZE];
	x86_64_virtual_memory_pg_el_cache x86_64_vmi_cache[CPU_X86_64_VIRTUAL_MEMORY_CACHE_TABLE_SIZE];

	uint64_t x86_64_statistic_vm_cache_access, x86_64_statistic_vm_cache_hit, x86_64_statistic_vm_cache_flush, x86_64_statistic_cycle_kernel, x86_64_statistic_cycle_user;
	
	void flush_internal_caches();

	inline uint64_t read_gen_reg_64(uint8_t code) {
		return BX_READ_64BIT_REG(code);
	}

	inline void write_gen_reg_64(uint8_t code, uint64_t v) {
		BX_WRITE_64BIT_REG(code, v);
	}

	void get_physical_memory_address(x86_64_virtual_memory_data *r, x86_64_virtual_memory_pg_el_cache *from_cache = NULL);

	inline void disable_interruption() {
		this->interrupt_enabled = 0;
	}

	inline void enable_interruption() {
		this->interrupt_enabled = 1;
	}

	inline uint8_t get_cpl() {
		return this->cpl;
	}

	inline void set_cpl(uint8_t v) {
		this->cpl = v;
	}

	void get_idt_el(uint8_t c, x86_84_int_vector_el *el);

	void go_to_kernel_mode(uint64_t start_pc);

	inline uint8_t io_inb(uint16_t port) {
		uint8_t data;
		this->io_space_read(&data, 1, port);
		#ifdef DEBUG_CPU
			DEBUG_PRINTF("INB  port=0x%X   value=0x%X\n", (uint32_t)port, (uint32_t)data);
		#endif
		return data;
	}

	inline uint16_t io_inw(uint16_t port) {
		uint16_t data;
		this->io_space_read(&data, 2, port);
		#ifdef DEBUG_CPU
			DEBUG_PRINTF("INW  port=0x%X   value=0x%X\n", (uint32_t)port, (uint32_t)data);
		#endif
		return data;
	}

	inline uint32_t io_ind(uint16_t port) {
		uint32_t data;
		this->io_space_read(&data, 4, port);
		#ifdef DEBUG_CPU
			DEBUG_PRINTF("IND  port=0x%X   value=0x%X\n", (uint32_t)port, (uint32_t)data);
		#endif
		return data;
	}

	inline void io_outb(uint16_t port, uint8_t data) {
		this->io_space_write(&data, 1, port);
		#ifdef DEBUG_CPU
			DEBUG_PRINTF("OUTB  port=0x%X   value=0x%X\n", (uint32_t)port, (uint32_t)data);
		#endif
	}

	inline void io_outw(uint16_t port, uint16_t data) {
		this->io_space_write(&data, 2, port);
		#ifdef DEBUG_CPU
			DEBUG_PRINTF("OUTW  port=0x%X   value=0x%X\n", (uint32_t)port, (uint32_t)data);
		#endif
	}

	inline void io_outd(uint16_t port, uint32_t data) {
		this->io_space_write(&data, 4, port);
		#ifdef DEBUG_CPU
			DEBUG_PRINTF("OUTD  port=0x%X   value=0x%X\n", (uint32_t)port, (uint32_t)data);
		#endif
	}

	inline uint64_t read_model_specific_reg(uint32_t code) {
		uint64_t value = (code < CPU_X8664_NUMBER_MSR) ? this->model_specific_regs[code]: 0;
		#ifdef DEBUG_CPU
			DEBUG_PRINTF("MSR read  %u    value=0x"PRINTF_INT64X_PREFIX" decimal="PRINTF_INT64_PREFIX"\n", (uint32_t)code, value, value);
		#endif
		return value;
	}

	inline void write_model_specific_reg(uint32_t code, uint64_t value) {
		if (code < CPU_X8664_NUMBER_MSR)
			this->model_specific_regs[code] = value;
		#ifdef DEBUG_CPU
			DEBUG_PRINTF("MSR write  %u    value=0x"PRINTF_INT64X_PREFIX" decimal="PRINTF_INT64_PREFIX"\n", (uint32_t)code, value, value);
		#endif
	}

	uint8_t bochs_mem_read_1_bytes(uint64_t vaddr);
	uint16_t bochs_mem_read_2_bytes(uint64_t vaddr);
	uint32_t bochs_mem_read_4_bytes(uint64_t vaddr);
	uint64_t bochs_mem_read_8_bytes(uint64_t vaddr);

	void bochs_mem_write_1_bytes(uint64_t vaddr, uint8_t data);
	void bochs_mem_write_2_bytes(uint64_t vaddr, uint16_t data);
	void bochs_mem_write_4_bytes(uint64_t vaddr, uint32_t data);
	void bochs_mem_write_8_bytes(uint64_t vaddr, uint64_t data);
	
	void bochs_mem_read_PackedXmmRegister(uint64_t vaddr, BxPackedXmmRegister *data);
	void bochs_mem_write_PackedXmmRegister(uint64_t vaddr, BxPackedXmmRegister *data);
	void bochs_mem_read_PackedXmmRegister_aligned(uint64_t vaddr, BxPackedXmmRegister *data);
	void bochs_mem_write_PackedXmmRegister_aligned(uint64_t vaddr, BxPackedXmmRegister *data);

	// bochs read-write-modify
	uint8_t bochs_mem_RMW_read_1_bytes(uint64_t vaddr);
	uint16_t bochs_mem_RMW_read_2_bytes(uint64_t vaddr);
	uint32_t bochs_mem_RMW_read_4_bytes(uint64_t vaddr);
	uint64_t bochs_mem_RMW_read_8_bytes(uint64_t vaddr);
	void bochs_mem_RMW_write_1_bytes(uint8_t data);
	void bochs_mem_RMW_write_2_bytes(uint16_t data);
	void bochs_mem_RMW_write_4_bytes(uint32_t data);
	void bochs_mem_RMW_write_8_bytes(uint64_t data);

	void mem_check_exc_read(uint64_t vaddr, uint8_t length, uint8_t& has_exception, exception_code_t& exc);
	void mem_check_exc_write(uint64_t vaddr, uint8_t length, uint8_t& has_exception, exception_code_t& exc);
	void mem_check_exc_read_write(uint64_t vaddr, uint8_t length, uint8_t& has_exception, exception_code_t& exc);

	void print_registers2(char *s);

	#ifdef X86_64_FAKE_SYSCALLS
		void video_initialize(uint32_t width, uint32_t height);
		void video_remove_line_from_buffer();
		void video_print_string(char *s);
		void read_elf64_header(uint32_t boot_size);
		void process_fake_syscall();
	#endif

#endif
