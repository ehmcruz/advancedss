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

#include "cpu.h"
#include "rs_policy.h"

using namespace cpu;

cpu::rs_policy_t::rs_policy_t(functional_units_t *f_units, uint32_t number_processors_shared)
	: cpu_shared_component_t(number_processors_shared)
{
	this->f_units = f_units;
}

cpu::rs_policy_just_one_t::rs_policy_just_one_t(functional_units_t *f_units, uint32_t number_processors_shared, uint32_t size)
    : rs_policy_t(f_units, number_processors_shared)
{
	this->list.set_list_size( size );
}

uint8_t cpu::rs_policy_just_one_t::add(rs_instruction_t *s)
{
	rs_instruction_static_list_el_t *lel = NULL;

	if (this->list.get_empty() > 0) {
		if (s->are_all_dep_resolved())
			s->set_status( CPU_INSTRUCTION_STATUS_DEP_READY );
		else
			s->set_status( CPU_INSTRUCTION_STATUS_WAIT_DEP );
		lel = this->list.add_end( s );
		s->set_rs_policy_list_el(lel);
	}

	return (lel != NULL);
}

uint8_t cpu::rs_policy_just_one_t::del(rs_instruction_t *s)
{
	/*uint32_t i, j;

	for (i=0; i<this->get_number_processors_shared(); i++) {
		for (j=0; j<r[i].n_instructions; j++) {
			this->list.del( r[i].instructions[j] );
		}
	}*/

	this->list.del( s->get_rs_policy_list_el() );

	return 1;
}

uint32_t cpu::rs_policy_just_one_t::get_capacity()
{
	return this->list.get_capacity();
}

void cpu::rs_policy_just_one_t::supply()
{
	rs_instruction_static_list_el_t *el;
	rs_instruction_t *s;

	#ifdef DEBUG_CPU
		DEBUG_PRINTF("RS queue:\n\n");
		for (el=this->list.get_first(); el!=NULL; el=el->get_next()) {
			s = el->get_data();
				DEBUG_PRINTF("\t%s        id=%u   status=%s   type=%s", s->get_unmounted_inst(), CONVERT64TOPRINT(s->get_id()), get_instruction_status_str(s->get_status()), get_inst_type_str(s->get_type()));
				if (s->get_status() == CPU_INSTRUCTION_STATUS_WAIT_DEP) {
					s->print_dependencies();
				}
				DEBUG_PRINTF("\n");
		}
	#endif

	for (el=this->list.get_first(); el!=NULL; el=el->get_next()) {
		s = el->get_data();
		if (s->get_status() == CPU_INSTRUCTION_STATUS_DEP_READY) {
			if (this->f_units->issue(s)) {
				s->set_status(CPU_INSTRUCTION_STATUS_EXECUTING);
			}
		}
	}

	this->get_cpu(0)->statistics.rs_queue_cumulative_occupancy += this->list.get_n_elements();
}

void cpu::rs_policy_just_one_t::flush(processor_t *c)
{
	rs_instruction_static_list_el_t *el, *next;
	rs_instruction_t *s;

	for (el=this->list.get_first(); el!=NULL; el=next) {
		s = el->get_data();
		next = el->get_next();
		if (s->get_cpu() == c) {
			this->list.del(el);
		}
	}
}

void cpu::rs_policy_just_one_t::flush_on_write_back(rs_instruction_t *s)
{
	rs_instruction_static_list_el_t *el, *next;
	rs_instruction_t *s_;
	processor_t *c;
	uint64_t id;
		
	c = s->get_cpu();
	
	id = s->get_id();

	for (el=this->list.get_first(); el!=NULL; el=next) {
		s_ = el->get_data();
		next = el->get_next();
		if (s_->get_cpu() == c && s_->get_id() > id) {
			this->list.del(el);
		}
	}
}

void cpu::rs_policy_just_one_t::arrived_to_commit(rs_instruction_t *s)
{
}

/****************************************************************/

cpu::rs_policy_io_queue_t::rs_policy_io_queue_t(functional_units_t *f_units, uint32_t number_processors_shared, uint32_t size)
    : rs_policy_just_one_t(f_units, number_processors_shared, size)
{
	uint32_t i;
	this->must_issue = (uint8_t*)malloc(number_processors_shared * sizeof(uint8_t));
	if (this->must_issue == NULL) {
		LOG_PRINTF("cpu::rs_policy_io_queue_t::rs_policy_io_queue_t malloc error\n");
		exit(1);
	}
	for (i=0; i<number_processors_shared; i++) {
		this->must_issue[i] = 0;
	}
}

void cpu::rs_policy_io_queue_t::supply()
{
	rs_instruction_static_list_el_t *el;
	rs_instruction_t *s;
	uint32_t cpu_vid;

	#ifdef DEBUG_CPU
		DEBUG_PRINTF("IO queue:  (");
		if (this->must_issue[ cpu_vid ]) {
			DEBUG_PRINTF("must issue)\n\n");
		}
		else {
			DEBUG_PRINTF("must NOT issue)\n\n");
		}
		for (el=this->list.get_first(); el!=NULL; el=el->get_next()) {
			s = el->get_data();
				DEBUG_PRINTF("\t%s        id=%u   status=%s   ", s->get_unmounted_inst(), CONVERT64TOPRINT(s->get_id()), get_instruction_status_str(s->get_status()));
				if (s->get_status() == CPU_INSTRUCTION_STATUS_WAIT_DEP) {
					s->print_dependencies();
				}
				DEBUG_PRINTF("\n");
		}
	#endif

	/* the first element found with the correct cpu is the ready-to-execute element */

	for (el=this->list.get_first(); el!=NULL; el=el->get_next()) {
		s = el->get_data();
		cpu_vid = s->get_cpu()->get_virtual_id();
		if (this->must_issue[ cpu_vid ]) {
			if (this->f_units->issue(s)) {
				this->must_issue[ cpu_vid ] = 0;
				s->set_status(CPU_INSTRUCTION_STATUS_EXECUTING);
			}

			break;
		}
	}

	this->get_cpu(0)->statistics.io_queue_cumulative_occupancy += this->list.get_n_elements();
}

void cpu::rs_policy_io_queue_t::arrived_to_commit(rs_instruction_t *s)
{
	this->must_issue[ s->get_cpu()->get_virtual_id() ] = 1;
}

void cpu::rs_policy_io_queue_t::flush(processor_t *c)
{
	this->must_issue[ c->get_virtual_id() ] = 0;

	this->rs_policy_just_one_t::flush(c);
}

void cpu::rs_policy_io_queue_t::flush_on_write_back(rs_instruction_t *s)
{
	this->rs_policy_just_one_t::flush_on_write_back(s);
}

/****************************************************************/

cpu::rs_policy_ls_queue_simple_t::rs_policy_ls_queue_simple_t(functional_units_t *f_units, uint32_t number_processors_shared, uint32_t size)
    : rs_policy_just_one_t(f_units, number_processors_shared, size)
{
	uint32_t i;
	this->write_ready = (uint8_t*)malloc(number_processors_shared * sizeof(uint8_t));
	if (this->write_ready == NULL) {
		LOG_PRINTF("cpu::rs_policy_ls_queue_t::rs_policy_ls_queue_t malloc error\n");
		exit(1);
	}
	for (i=0; i<number_processors_shared; i++) {
		this->write_ready[i] = 0;
	}
}

void cpu::rs_policy_ls_queue_simple_t::supply()
{
	rs_instruction_static_list_el_t *el;
	rs_instruction_t *s;
	uint32_t cpu_vid;

	#ifdef DEBUG_CPU
		DEBUG_PRINTF("LS queue:  (");
		if (this->write_ready[ cpu_vid ]) {
			DEBUG_PRINTF("must issue write)\n\n");
		}
		else {
			DEBUG_PRINTF("must issue normal)\n\n");
		}
		for (el=this->list.get_first(); el!=NULL; el=el->get_next()) {
			s = el->get_data();
				DEBUG_PRINTF("\t%s        id=%u   status=%s   type=%s", s->get_unmounted_inst(), CONVERT64TOPRINT(s->get_id()), get_instruction_status_str(s->get_status()), get_inst_type_str(s->get_type()));
				if (s->get_status() == CPU_INSTRUCTION_STATUS_WAIT_DEP) {
					s->print_dependencies();
				}
				DEBUG_PRINTF("\n");
		}
	#endif

	/* In this simple load/store policy we just:
	   - execute stores when they arrive to commit
	   - execute loads when there were no stores before them
	*/

	/* search for a load, stop if we find a store and this->write_ready[ cpu_vid ] is not set */
	for (el=this->list.get_first(); el!=NULL; el=el->get_next()) {
		s = el->get_data();
		cpu_vid = s->get_cpu()->get_virtual_id();
		if (s->get_type() == CPU_INSTRUCTION_TYPE_STORE && this->write_ready[ cpu_vid ] == 0)
			continue;
		if (s->get_status() == CPU_INSTRUCTION_STATUS_DEP_READY) {
			if (this->f_units->issue(s)) {
				s->set_status(CPU_INSTRUCTION_STATUS_EXECUTING);
				this->write_ready[ cpu_vid ] = 0;
			}
			else { // all LOAD/STORE units are full
				break;
			}
		}
	}

	this->get_cpu(0)->statistics.ls_queue_cumulative_occupancy += this->list.get_n_elements();
}

void cpu::rs_policy_ls_queue_simple_t::arrived_to_commit(rs_instruction_t *s)
{
	if (s->get_type() == CPU_INSTRUCTION_TYPE_STORE) {
		this->write_ready[ s->get_cpu()->get_virtual_id() ] = 1;
	}
}

void cpu::rs_policy_ls_queue_simple_t::flush(processor_t *c)
{
	this->write_ready[ c->get_virtual_id() ] = 0;

	this->rs_policy_just_one_t::flush(c);
}

void cpu::rs_policy_ls_queue_simple_t::flush_on_write_back(rs_instruction_t *s)
{
	this->rs_policy_just_one_t::flush_on_write_back(s);
}
