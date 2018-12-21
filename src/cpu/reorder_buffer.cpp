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
#include "../io/io.h"

namespace cpu {
	extern uint8_t cfg_detect_branch_misprediction_on_wb;
};

using namespace cpu;

/***********************************************************************/

cpu::reorder_buffer_t::reorder_buffer_t(uint32_t size, uint32_t number_processors_shared)
	: cpu_shared_component_t(number_processors_shared)
{
	uint32_t i, must_alloc;
	rs_instruction_t *s;

	this->list_inst_supplier.set_list_size( size );
	this->list.set_list_size( size );

	for (i=0; i<size; i++) {
		s = new rs_instruction_t;
		must_alloc = size * CPU_INSTRUCTION_MAX_DEP_IN;
		s->set_dep_out_buffer( must_alloc );
		this->list_inst_supplier.add_end(s);
	}

	this->must_flush = (uint8_t*)malloc(sizeof(uint8_t)*number_processors_shared);
	SANITY_ASSERT(this->must_flush != NULL);
}

rs_instruction_t* cpu::reorder_buffer_t::get_inst_from_supply()
{
	rs_instruction_t *s;

	if (this->list_inst_supplier.get_n_elements() > 0) {
		s = this->list_inst_supplier.get_first()->get_data();
		this->list_inst_supplier.del_first();
	}
	else
		s = NULL;

	return s;
}

void cpu::reorder_buffer_t::add(rs_instruction_t *s)
{
	rs_instruction_static_list_el_t *lel;
	lel = this->list.add_end(s);
	s->set_rb_list_el(lel);
}

void cpu::reorder_buffer_t::commit()
{
	rs_instruction_static_list_el_t *el, *next;
	rs_instruction_t *s;
	processor_t *c;
	uint32_t i;

	for (i=0; i<this->get_number_processors_shared(); i++) {
		this->must_flush[i] = 0;
	}
	
	#ifdef SANITY_ASSERT
		for (el=this->list.get_first(); el!=NULL; el=next) {
			s = el->get_data();
			next = el->get_next();
			if (s->get_status() == CPU_INSTRUCTION_STATUS_WAIT_DEP) {
				LOG_PRINTF("a instrução deu dead-lock addr=0x%llX id=%llu, dep_in_resolved=%u, dep_in=%u\n", s->get_vaddr(), s->get_id(), s->get_dep_in_resolved(), s->get_dep_in_n());
				exit(1);
			}
			break;
		}
	#endif

	#ifdef DEBUG_CPU
		DEBUG_PRINTF("Reorder-buffer:\n\n");
		for (el=this->list.get_first(); el!=NULL; el=next) {
			s = el->get_data();
			next = el->get_next();
			DEBUG_PRINTF("\t%s        id=%u   status=%s  vaddr=0x%llX\n", s->get_unmounted_inst(), CONVERT64TOPRINT(s->get_id()), get_instruction_status_str(s->get_status()), s->get_vaddr());
		}
	#endif

	for (el=this->list.get_first(); el!=NULL; el=next) {
		s = el->get_data();
		next = el->get_next();
		c = s->get_cpu();

		if (s->get_status() == CPU_INSTRUCTION_STATUS_READY) {
			if (s->get_microcode_state() == RS_INSTRUCTION_MICROCODE_FIRST) {
				c->statistics.commited_inst++;
			}
		
			#ifdef DEBUG_CPU
				DEBUG_PRINTF("COMMITTING INSTRUCTION   %s        id=%u\n", s->get_unmounted_inst(), CONVERT64TOPRINT(s->get_id()));
			#endif

			if (s->has_exception()) {
				c->process_exception(s->get_exception_code(), s->get_vaddr(), s->get_length());
				#ifdef DEBUG_CPU
					DEBUG_PRINTF("\tflushing pipeline due to exception\n");
				#endif

				this->must_flush[c->get_virtual_id()] = 1;
				c->flush_pipeline_others();
			}
			else if (s->get_is_trap()) {
				c->process_trap(s->get_trap_code(), s->get_vaddr(), s->get_length());
				#ifdef DEBUG_CPU
					DEBUG_PRINTF("\tflushing pipeline due to trap\n");
				#endif

				this->must_flush[c->get_virtual_id()] = 1;
				c->flush_pipeline_others();
			}
			else if (s->get_is_halt()) {
				#ifdef DEBUG_CPU
					DEBUG_PRINTF("\tflushing pipeline due to halt instruction\n");
				#endif

				this->must_flush[c->get_virtual_id()] = 1;
				advancedss_stop_simulation();
			}
			else if (s->must_flush_pipeline()) {
				DEBUG_PRINTF("\tinstruction must flush pipeline\n");
				c->process_instruction_that_flushes_pipeline();
				this->must_flush[c->get_virtual_id()] = 1;
				c->flush_pipeline_others();
			}
			else if (s->get_is_branch()) {
				branch_el_t branch;
				#ifdef DEBUG_CPU
					DEBUG_PRINTF("\tis a branch\n");
				#endif

				branch.mem_addr = s->get_vaddr() + s->get_length() - 1;
				branch.target_addr = s->get_branch_target();
				branch.taken = s->get_branch_taken();
				branch.is_call = 0;
				branch.is_ret = 0;

				c->warn_branch_predictor(&branch);
				
				if (!cfg_detect_branch_misprediction_on_wb) {
					if (!s->branch_mis_predicted()) {
						c->statistics.branch_pred_correct++;
					}
					c->statistics.branch_pred_total++;
				}
			}

			if (s->branch_mis_predicted()) { // Delete the next instructions of the pipeline
				#ifdef DEBUG_CPU
					DEBUG_PRINTF("\tflushing pipeline due to branch misprediction\n");
				#endif

				this->must_flush[c->get_virtual_id()] = 1;
				
				if (s->get_type() != CPU_INSTRUCTION_TYPE_BRANCH)
					c->statistics.branch_missing++;
//					LOG_PRINTF("falta 0x%llX\n", s->get_vaddr());
			}
			#ifdef DEBUG_CPU
			else {
				DEBUG_PRINTF("\tbranch ok!!!\n");
			}
			#endif

			c->statistics.committed_uops[ s->get_type() ]++;

			if (this->must_flush[c->get_virtual_id()])
				break;

			this->list.del(el);
			this->list_inst_supplier.add_end( s );
		}
		else {
			if (s->get_status() == CPU_INSTRUCTION_STATUS_DEP_READY) {
				c->arrived_to_commit(s);
			}
			break;
		}
	}

	for (i=0; i<this->get_number_processors_shared(); i++) {
		if (this->must_flush[i]) {
			this->flush( this->get_cpu(i) );
		}
	}

	this->get_cpu(0)->statistics.rb_cumulative_occupancy += this->list.get_n_elements();
}

void cpu::reorder_buffer_t::flush(processor_t *c)
{
	rs_instruction_t *decoder_buffer[CPU_DECODER_MAX_ALLOCATED_INST];
	uint8_t decoder_buffer_n, i;
	rs_instruction_static_list_el_t *el, *next;
	rs_instruction_t *s;
	
	decoder_buffer_n = c->flush_pipeline_simple(decoder_buffer);

	#ifdef DEBUG_CPU
		DEBUG_PRINTF("\tthere were %u instructions in the decoder\n", (uint32_t)decoder_buffer_n);
	#endif

	for (i=0; i<decoder_buffer_n; i++) {
		this->list_inst_supplier.add_end( decoder_buffer[i] );
	}
	
	for (el=this->list.get_first(); el!=NULL; el=next) {
		s = el->get_data();
		next = el->get_next();
		if (s->get_cpu() == c) {
			this->list_inst_supplier.add_end( s );
			this->list.del(el);
		}
	}
}

void cpu::reorder_buffer_t::flush_on_write_back(rs_instruction_t *s)
{
	rs_instruction_static_list_el_t *el, *next;
	rs_instruction_t *s_;
	processor_t *c;
	rs_instruction_t *decoder_buffer[CPU_DECODER_MAX_ALLOCATED_INST];
	uint8_t decoder_buffer_n, i;
	
	c = s->get_cpu();
	
	decoder_buffer_n = c->flush_pipeline_on_wb(decoder_buffer, s);

	#ifdef DEBUG_CPU
		DEBUG_PRINTF("\tthere were %u instructions in the decoder\n", (uint32_t)decoder_buffer_n);
	#endif

	for (i=0; i<decoder_buffer_n; i++) {
		this->list_inst_supplier.add_end( decoder_buffer[i] );
	}

	el = s->get_rb_list_el();

	if (el != NULL)	 {
		for (el=el->get_next(); el!=NULL; el=next) {
			s_ = el->get_data();
			next = el->get_next();
			if (s_->get_cpu() == c) {
				s_->flush_on_write_back();
				this->list_inst_supplier.add_end( s_ );
				this->list.del(el);
			}
		}
	}
}
