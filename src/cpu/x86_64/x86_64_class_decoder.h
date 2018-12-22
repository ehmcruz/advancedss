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

#ifndef _ADVANCEDSS_X8664_INC_DECODER_HEADER_
#define _ADVANCEDSS_X8664_INC_DECODER_HEADER_

protected:
	struct x86_64_decoder_state_t {
		uint8_t opcode_bytes, first_opcode, second_opcode, has_b3, addr_size, modrm_memory_operand;
		uint32_t bochs_opcode, bochs_offset, bochs_attr, bochs_rex_prefix, bochs_b3, bochs_sse_prefix;
		uint8_t has_modrm, modrm, modrm_disp_bytes;
		uint8_t has_sib, sib;
		uint8_t disp_already_fetched;
		uint8_t im_already_fetched;
		uint8_t im_bytes;
		uint8_t prefix_number, required_rs;
		uint8_t modrm_dep_base, modrm_dep_index, modrm_has_dep_base, modrm_has_dep_index;

		const x8664_dep_rs_num_type_t *dep_rs_num_type;
		const x8664_dep_call_table_t *dep_call_tb;

		bxInstruction_c bochs_inst;

		#ifdef DEBUG_CPU
			char modrm_debug[50];
		#endif

		uint8_t busy;
		uint64_t vaddr;
		uint8_t length;
	};

	uint8_t is_first_byte;
	uint32_t bytes_to_get, bytes_already_got;

	x86_64_decoder_state_t *state;

	x86_64_decoder_state_t inst_cache_tb[CPU_X86_64_DECODER_INST_CACHE_TABLE_SIZE];

public:
	uint64_t statistic_instructions, statistic_instructions_invalid;

	uint64_t x86_64_statistic_cache_access, x86_64_statistic_cache_hit, x86_64_statistic_cache_flush;

	void x86_64_decode();
	void x86_64_get_second_opcode();
	void x86_64_after_get_opcodes();
	void x86_64_get_modrm();
	void x86_64_get_modrm_3byte();
	void x86_64_after_get_modrm();
	void x86_64_modrm_done();
	void x86_64_get_sib();
	void x86_64_get_disp();
	void x86_64_get_im();
	void x86_64_get_im_();
//	void x86_64_invalid_opcode();
	void x86_64_after_get_inst();
	void x86_64_after_allocate_rs();
	void x86_64_flush_inst_cache();
	void x86_64_get_bytes();

	void implement_inst_invalid();
	void implement_inst_16bits_invalid();
	void implement_inst_32bits_invalid();
	void implement_inst_64bits_invalid();

	#define NO_LOOKUP(IDENT, N, TYPES) \
		void implement_inst_16bits_##IDENT (); \
		void implement_inst_32bits_##IDENT () ;\
		void implement_inst_64bits_##IDENT () ;

	#define NO_LOOKUP_(IDENT, N, TYPES)
	#define INVALID
	#define LOOKUP(TB)
	#define LIST_TYPES(...)

	#include "dep.h"

	#undef NO_LOOKUP
	#undef NO_LOOKUP_
	#undef LOOKUP
	#undef INVALID
	#undef LIST_TYPES

#endif
