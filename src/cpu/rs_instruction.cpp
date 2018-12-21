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

#include <stdlib.h>
#include <stdio.h>
#include "cpu.h"

using namespace cpu;

void cpu::rs_instruction_t::reset(processor_t *c, uint64_t id)
{
	this->my_cpu = c;
	this->id = id;
	this->is_branch = 0;
	this->is_trap = 0;
	this->out_deps_regs_n = 0;
	this->dep_out_n = 0;
	this->dep_in_n = 0;
	this->dep_in_resolved = 0;
	this->cycles_to_exec = 0;
	this->status = CPU_INSTRUCTION_STATUS_DECODING;
	this->exception = 0;
	this->branch_foresight = 1; // updated later if is branch
	this->type = 0;
	this->memory_result = 0;
	this->is_halt = 0;
	this->inst_must_flush_pipeline = 0;
	this->rs_policy = NULL;
	this->rs_policy_list_el = NULL;
	this->rb_list_el = NULL;
	#ifdef DEBUG_CPU
		strcpy(this->unmounted_inst, "invalid_instruction");
	#endif
}

void cpu::rs_instruction_t::set_dep_out_buffer(uint32_t dep_out_max)
{
	this->dep_out = (rs_instruction_t**)malloc( sizeof(rs_instruction_t*) * dep_out_max );
	SANITY_ASSERT(this->dep_out != NULL);
	this->dep_out_max = dep_out_max;
	this->dep_out_index = (uint32_t*)malloc(sizeof(uint32_t) * dep_out_max);
	SANITY_ASSERT(this->dep_out_index != NULL);
}

void cpu::rs_instruction_t::create_dep(rs_instruction_t *dest, rs_instruction_t *src)
{
	#ifdef SANITY_CHECK
		SANITY_ASSERT(dest->dep_in_n < CPU_INSTRUCTION_MAX_DEP_IN);
		SANITY_ASSERT(src->dep_out_n < src->dep_out_max);
	#endif

	src->dep_out_index[src->dep_out_n] = dest->dep_in_n;
	dest->dep_in_index[dest->dep_in_n] = src->dep_out_n;

	dest->dep_in[ dest->dep_in_n++ ] = src;
	src->dep_out[ src->dep_out_n++ ] = dest;

	#ifdef DEBUG_CPU
		DEBUG_PRINTF("cpu::rs_instruction_t::create_dep   Instruction (%s) id=%u set as dependency to (%s) id=%u\n", src->get_unmounted_inst(), CONVERT64TOPRINT(src->get_id()), dest->get_unmounted_inst(), CONVERT64TOPRINT(dest->get_id()));
	#endif
}

void cpu::rs_instruction_t::write_back()
{
	uint32_t i, pos;
	rs_instruction_t *s;

//	this->set_cycle_ready( CPU_GET_CYCLE(this->my_cpu) ); // used to simulate write-back stage
	this->set_status( CPU_INSTRUCTION_STATUS_READY );

	/* Delete the instructions dependencies
	*/

	for (i=0; i<this->dep_out_n; i++) {
//		SANITY_ASSERT( this->dep_out[ i ]->dep_in[ this->dep_out_index[ i ] ] == this );
		pos = this->dep_out_index[ i ];
		s = this->dep_out[ i ];

		// delete the dependency from the dep_in of the target instruction

		if (s != NULL) {
			s->dep_in[pos] = NULL;
//			if (s->get_id() == 1320379) { LOG_PRINTF("wb na inst, eu sou %llu\n", this->id); }
			if (++s->dep_in_resolved >= s->dep_in_n)
				s->set_status( CPU_INSTRUCTION_STATUS_DEP_READY );
			this->dep_out[ i ] = NULL;
		}
	}

	/* Check in register bank if this instruction is still the
	   result generator. If it is, set the register as ready
	*/

	for (i=0; i<this->out_deps_regs_n; i++) {
		if (*this->out_deps_regs[i] == this)
			*this->out_deps_regs[i] = NULL;
	}
}

void cpu::rs_instruction_t::flush_on_write_back()
{
	uint32_t i, pos;
	rs_instruction_t *s;

	/* Delete the instructions dependencies
	*/

	for (i=0; i<this->dep_in_n; i++) {
//		SANITY_ASSERT( this->dep_out[ i ]->dep_in[ this->dep_out_index[ i ] ] == this );
		pos = this->dep_in_index[ i ];
		s = this->dep_in[ i ];

		if (s != NULL) {
			s->dep_out[pos] = NULL;
			this->dep_in[ i ] = NULL;
		}
	}
}

#ifdef DEBUG_CPU
	void cpu::rs_instruction_t::print_dependencies()
	{
		uint8_t i;
		for (i=0; i<this->dep_in_n; i++) {
			if (this->dep_in[i] != NULL) {
				DEBUG_PRINTF("%u(%s) - ", this->dep_in[i]->get_id(), this->dep_in[i]->get_unmounted_inst());
			}
		}
	}
#endif
