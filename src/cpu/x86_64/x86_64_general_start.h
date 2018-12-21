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

#ifndef _ADVANCEDSS_X8664_GENERAL_START_HEADER_
#define _ADVANCEDSS_X8664_GENERAL_START_HEADER_

#include "../../config.h"

/****************************************/

/* used by advancedss core */

typedef uint64_t target_addr_t;
typedef uint64_t exception_code_t;
typedef uint16_t trap_code_t;

#define CPU_DECODER_MAX_OPCODE_BYTES       15
#define CPU_INSTRUCTION_MAX_DEP_IN         10
#define CPU_INSTRUCTION_MAX_DEP_OUT        10
#define CPU_DECODER_MAX_ALLOCATED_INST     7

/****************************************/

/* used by x86-64 */

#define CPU_X86_64_DECODER_INST_CACHE_TABLE_BITS                   14

/***********************************************************/
// don't touch these
#define CPU_X86_64_DECODER_INST_CACHE_TABLE_SIZE                   (1 << CPU_X86_64_DECODER_INST_CACHE_TABLE_BITS)
#define CPU_X86_64_DECODER_INST_CACHE_TABLE_MASK                   (CPU_X86_64_DECODER_INST_CACHE_TABLE_SIZE - 1)
#define CPU_X86_64_DECODER_INST_CACHE_TABLE_GET_INDEX(VADDR)       ((VADDR) & CPU_X86_64_DECODER_INST_CACHE_TABLE_MASK)
/***********************************************************/

#define CPU_X86_64_VIRTUAL_MEMORY_CACHE_TABLE_BITS                   13

/***********************************************************/
// don't touch these
#define CPU_X86_64_VIRTUAL_MEMORY_CACHE_TABLE_SIZE                   (1 << CPU_X86_64_VIRTUAL_MEMORY_CACHE_TABLE_BITS)
#define CPU_X86_64_VIRTUAL_MEMORY_CACHE_TABLE_MASK                   (CPU_X86_64_VIRTUAL_MEMORY_CACHE_TABLE_SIZE - 1)
#define CPU_X86_64_VIRTUAL_MEMORY_CACHE_TABLE_GET_INDEX(VADDR)       ((VADDR) & CPU_X86_64_VIRTUAL_MEMORY_CACHE_TABLE_MASK)
/***********************************************************/

// not implemented yet
//#define CPU_X86_64_DECODER_REQUIRE_15_BYTES

struct bochs_exec_status_t;

#include "bochs/general_start.h"
#include "dependency.h"

struct bochs_exec_status_t {
/*	uint8_t is_branch;
	uint8_t branch_taken;
	uint64_t branch_target;*/
	uint8_t has_exception;
	exception_code_t exception_code;
	uint8_t is_trap;
	trap_code_t trap_code;
	uint8_t is_halt;
	uint8_t must_flush_pipeline;
	bxInstruction_c *bochs_inst;
};

char* x86_64_get_gpr64_name(uint8_t code);
char* x86_64_get_gpr32_name(uint8_t code);
char* x86_64_get_gpr16_name(uint8_t code);
char* x86_64_get_gpr8_name(uint8_t code, uint8_t rex);
char* x86_64_get_rflags_name(uint8_t code);

#define CPU_X8664_NUMBER_MSR                   16

#define CPU_X8664_MSR_NUMBER_PHYSICAL_CPU             0
#define CPU_X8664_MSR_NUMBER_VIRTUAL_CPU_PER_PHY      1
#define CPU_X8664_MSR_CPU_ID                          2
#define CPU_X8664_MSR_KERNEL_STACK_PTR                3
#define CPU_X8664_MSR_PHYSICAL_MEMORY_SIZE            4
#define CPU_X8664_MSR_VIRTUAL_MEMORY_CFG              5
#define CPU_X8664_MSR_VIRTUAL_MEMORY_PAGE_FAULT_ADDR  6
#define CPU_X8664_MSR_INTERRUPT_VECTOR_ADDR           7
#define CPU_X8664_MSR_SYSCALL_HANDLER_ADDR            8

#define CPU_X8664_DEP_VECTOR_GPR_BITS_0_7            	0
#define CPU_X8664_DEP_VECTOR_GPR_BITS_8_15              1
#define CPU_X8664_DEP_VECTOR_GPR_BITS_16_63             2
#define CPU_X8664_DEP_VECTOR_RFLAGS                     3
#define CPU_X8664_DEP_VECTOR_MSR                        4

#define CPU_X8664_CPL_KERNEL                        0
#define CPU_X8664_CPL_USER                          3

union x86_64_virtual_memory_cfg {
	struct {
		uint64_t base_addr: 40; // address is aligned to 12 bits
		uint64_t paging_enable: 1;
	} mask;

	uint64_t msr;
};

/* check amd64 manual volume 2 for details about the paging structures */

union x86_64_virtual_memory_pg_el {
	struct {
		uint64_t p: 1;
		uint64_t rw: 1;
		uint64_t us: 1;
		uint64_t pwt: 1;
		uint64_t pcd: 1;
		uint64_t a: 1;
		uint64_t d: 1;
		uint64_t pat: 1;
		uint64_t g: 1;
		uint64_t avl: 3;
		uint64_t addr: 40;
		uint64_t available: 11;
		uint64_t nx: 1;
	} mask;

	uint64_t data;
};

struct x86_64_virtual_memory_data {
	uint64_t vaddr;
	uint32_t paddr;

	uint8_t present: 1;
	uint8_t read: 1;
	uint8_t write: 1;
	uint8_t execute: 1;

	x86_64_virtual_memory_pg_el page;
	uint32_t page_el_addr;
};

struct x86_64_virtual_memory_data2 {
	x86_64_virtual_memory_data first_page;
	x86_64_virtual_memory_data second_page;
	uint8_t cross_boundary;
	uint64_t vaddr, last_vaddr_second_page;
	uint8_t exception_read_check;
	uint8_t exception_write_check;
	uint8_t length;
};

union x86_84_int_vector_el {
	struct {
		uint64_t handler_addr: 62; // handler address is aligned to 4 bytes
		uint64_t permit_software: 1; // allow for software interrupt
	} mask;

	uint64_t data;
};

#endif
