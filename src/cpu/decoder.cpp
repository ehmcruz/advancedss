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
//#include "../config.h"
//uint64_t inst_aa_a;

#include "cpu.h"

using namespace cpu;

#include "decoder_cpp.h"

cpu::decoder_t::decoder_t(processor_t *my_cpu)
{
	this->my_cpu = my_cpu;
	//this->mmu = my_cpu->get_mmu();
	this->inst_id = 0;
	this->fb = my_cpu->get_fetch_buffer();
	this->flush();
	this->initialize_decoder_arch();
}

void cpu::decoder_t::flush()
{
	this->mis_speculation = 0;
	this->allocated_rs_slots = 0;
	this->required_rs_slots = 0;
	this->dispatched_rs_slots = 0;
	this->status = CPU_DECODER_STATUS_IDLE;
}

#ifdef CPU_CHECK_EXECUTION_TRACE
	void cpu::decoder_t::decode(volatile uint64_t v)
#else
	void cpu::decoder_t::decode()
#endif
{
	#ifdef DEBUG_CPU
		if (!ENABLED_EXEC) {
			DEBUG_PRINTF("INSTRUCTION EXECUTION DISABLED UNTIL PIPELINE FLUSH\n");
		}
	#endif
	if (this->status == CPU_DECODER_STATUS_IDLE) {
		this->status = CPU_DECODER_STATUS_BUSY;
		this->branch_foresight = 0;
		this->allocated_rs_slots = 0;
		this->required_rs_slots = 0;
		this->dispatched_rs_slots = 0;
		this->inst_opcodes_i = 0;
		this->exception = 0;
		this->is_trap = 0;
		this->is_branch = 0;
		this->is_halt = 0;
		this->inst_must_flush_pipeline = 0;
		this->inst_unexpected_behavior = 0;
		this->check_if_branch_instruction[CPU_INSTRUCTION_TYPE_BRANCH] = 0;
		this->branch_automatic_detect = 0;
		DEBUG_PRINTF("start decoding new instruction\n");
		SET_HANDLER(initialize_instruction)
		this->initialize_instruction();
	}
	else {
		DEBUG_PRINTF("continue decoding previous instruction\n");
		EXEC_HANDLER
	}
}

void cpu::decoder_t::alocate_inst_()
{
	rs_instruction_t *s;

	while (this->allocated_rs_slots < this->required_rs_slots) {
		s = this->my_cpu->get_inst_from_buffer();
		if (s != NULL) {
			#ifdef DEBUG_CPU
				DEBUG_PRINTF("cpu::decoder_t::alocate_inst_  reservation stations %u of %u allocated\n", (uint32_t)this->allocated_rs_slots+1, (uint32_t)this->required_rs_slots);
			#endif
			s->reset(this->my_cpu, this->inst_id++);
			this->buffered_inst[ this->allocated_rs_slots ] = s;
			this->allocated_rs_slots++;
		}
		else {
			this->stop_decoding_in_cycle = 1;
			return;
		}
	}

	this->handler = this->handler_alocate_inst;
	EXEC_HANDLER
}

void cpu::decoder_t::alocate_inst(uint32_t n)
{
	#ifdef DEBUG_CPU
		DEBUG_PRINTF("cpu::decoder_t::alocate_inst  must allocate %u reservation stations\n", n);
	#endif

	this->required_rs_slots = n;
	this->handler_alocate_inst = this->handler;
	SET_HANDLER(alocate_inst_)
	this->alocate_inst_();
}

void cpu::decoder_t::dispatch()
{
	rs_instruction_t *s;

	while (this->dispatched_rs_slots < this->required_rs_slots) {
		s = this->buffered_inst[ this->dispatched_rs_slots ];
		if (this->my_cpu->dispatch(s)) {
//			inst_aa_a = this->inst_id;
			s->setup_for_dispatch(this->inst_addr, this->inst_length);
			this->dispatched_rs_slots++;
			#ifdef DEBUG_CPU
				DEBUG_PRINTF("cpu::decoder_t::dispatch  reservation stations %u of %u dispatched id="PRINTF_UINT64_PREFIX" %s\n", (uint32_t)this->dispatched_rs_slots, (uint32_t)this->required_rs_slots, s->get_id(), s->get_unmounted_inst());
			#endif
		}
		else {
			this->stop_decoding_in_cycle = 1;
			return;
		}
	}
	
	this->my_cpu->statistics.average_uops_per_inst += this->required_rs_slots;
	this->my_cpu->statistics.average_uops_per_inst_i++;

	this->status = CPU_DECODER_STATUS_IDLE;

	#ifdef DEBUG_CPU
		DEBUG_PRINTF("cpu::decoder_t::dispatch  instruction dispatched\n");
	#endif
}

uint8_t cpu::decoder_t::get_buffered_rs(rs_instruction_t **s)
{
	int8_t i, j;
	j = 0;
	for (i=this->allocated_rs_slots-1; i>=this->dispatched_rs_slots; i--)
		s[j++] = this->buffered_inst[i];
	return this->allocated_rs_slots - this->dispatched_rs_slots;
}

void cpu::decoder_t::fetch_buffer_exception()
{
	CPU_DECODER_INSTRUCTION_EXCEPTION(this->buffer_exception_code)
	INSTRUCTION_END
}
