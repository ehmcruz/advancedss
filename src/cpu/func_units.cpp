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

cpu::functional_units_t::functional_units_t(uint32_t number_processors_shared)
	: cpu_shared_component_t(number_processors_shared)
{
	uint32_t total, int_gen, int_alu, int_mul, int_div, branch, transfer, loadstore;
	uint32_t i, j, n;
	advancedss_config *cfg = get_advancedss_main_config();

	int_gen = cfg->read_int32("func_units_int_general");
	int_alu = cfg->read_int32("func_units_int_alu");
	int_mul = cfg->read_int32("func_units_int_mul");
	int_div = cfg->read_int32("func_units_int_div");
	transfer = cfg->read_int32("func_units_transfer");
	branch = cfg->read_int32("func_units_branch");
	loadstore = cfg->read_int32("func_units_loadstore");

	total = int_gen + int_alu + int_mul +  int_div + branch + transfer + loadstore + 1; // 1 -> io

	this->n_units = total;
	this->n_units_ls = loadstore;

	this->response = (functional_units_response_t*) malloc (sizeof(functional_units_response_t) * number_processors_shared);
	if (!this->response) {
		LOG_PRINTF("error malloc cpu::functional_units_t::functional_units_t vla\n");
		exit(1);
	}

	for (i=0; i<number_processors_shared; i++) {
		this->response[i].vid = i;
		this->response[i].instructions = (rs_instruction_t**)malloc(sizeof(rs_instruction_t*) * total);
		SANITY_ASSERT(this->response[i].instructions != NULL);
	}

	this->units = (func_unit_t*) malloc( sizeof(func_unit_t) * total );
	if (this->units == NULL) {
		LOG_PRINTF("error: cpu::functional_units_t::functional_units_t allocating memory\n");
		exit(1);
	}

	i = 0;
	j = 0;
	while (j<int_alu) {
		this->units[i].inst = NULL;
		this->units[i].inst_type = CPU_INST_TYPE_MASK(CPU_INSTRUCTION_TYPE_INT_ARITH)
		                           | CPU_INST_TYPE_MASK(CPU_INSTRUCTION_TYPE_INT_LOGIC);
		this->units[i].cycles = 0;

		i++;
		j++;
	}

	j = 0;
	while (j<int_mul) {
		this->units[i].inst = NULL;
		this->units[i].inst_type = CPU_INST_TYPE_MASK(CPU_INSTRUCTION_TYPE_INT_MUL);
		this->units[i].cycles = 0;

		i++;
		j++;
	}

	j = 0;
	while (j<int_div) {
		this->units[i].inst = NULL;
		this->units[i].inst_type = CPU_INST_TYPE_MASK(CPU_INSTRUCTION_TYPE_INT_DIV);
		this->units[i].cycles = 0;

		i++;
		j++;
	}

	j = 0;
	while (j<int_gen) {
		this->units[i].inst = NULL;
		this->units[i].inst_type = CPU_INST_TYPE_MASK(CPU_INSTRUCTION_TYPE_INT_ARITH)
		                           | CPU_INST_TYPE_MASK(CPU_INSTRUCTION_TYPE_INT_LOGIC)
		                           | CPU_INST_TYPE_MASK(CPU_INSTRUCTION_TYPE_INT_MUL)
		                           | CPU_INST_TYPE_MASK(CPU_INSTRUCTION_TYPE_INT_DIV);
		this->units[i].cycles = 0;

		i++;
		j++;
	}

	j = 0;
	while (j<branch) {
		this->units[i].inst = NULL;
		this->units[i].inst_type = CPU_INST_TYPE_MASK(CPU_INSTRUCTION_TYPE_BRANCH);
		this->units[i].cycles = 0;

		i++;
		j++;
	}

	j = 0;
	while (j<transfer) {
		this->units[i].inst = NULL;
		this->units[i].inst_type = CPU_INST_TYPE_MASK(CPU_INSTRUCTION_TYPE_MOV);
		this->units[i].cycles = 0;

		i++;
		j++;
	}

	/***************************
	 load/store and io units should be ALWAYS the last one's
	***************************/

	j = 0;
	while (j<loadstore) {
		this->units[i].inst = NULL;
		this->units[i].inst_type = CPU_INST_TYPE_MASK(CPU_INSTRUCTION_TYPE_LOAD)
		                           | CPU_INST_TYPE_MASK(CPU_INSTRUCTION_TYPE_STORE);
		this->units[i].cycles = 0;

		i++;
		j++;
	}

	this->units[i].inst = NULL;
	this->units[i].inst_type = CPU_INST_TYPE_MASK(CPU_INSTRUCTION_TYPE_IO_IN)
	                           | CPU_INST_TYPE_MASK(CPU_INSTRUCTION_TYPE_IO_OUT);
	this->units[i].cycles = 0;

	i++;
	
	SANITY_ASSERT(this->n_units == i);
	
	// create units quick access structure
	
	for (i=0; i<32; i++) {
		func_unit_t **f;
		n = 0;
		for (j=0; j<this->n_units; j++) {
			if (this->units[j].inst_type & CPU_INST_TYPE_MASK(i)) {
				n++;
			}
		}
		this->units_access[i] = (func_unit_t**)malloc( (n+1) * sizeof(func_unit_t*) );
		SANITY_ASSERT(this->units_access[i] != NULL);
		f = this->units_access[i];
		n = 0;
		for (j=0; j<this->n_units; j++) {
			if (this->units[j].inst_type & CPU_INST_TYPE_MASK(i)) {
				f[n++] = &this->units[j];
			}
		}
		f[n] = NULL;
	}
}

uint8_t cpu::functional_units_t::issue(rs_instruction_t *inst)
{
	uint32_t r;
	func_unit_t **f, *u;

/*	uint32_t t, i;
	t = CPU_INST_TYPE_MASK( inst->get_type() );*/

	
	r = 0;

	for (f=this->units_access[inst->get_type()], u=*f; u != NULL; u=*(++f)) {
		if (u->inst == NULL) {
			#ifdef DEBUG_CPU
				DEBUG_PRINTF("\tISSUE INSTRUCTION   %s        id=%u    para func_unit ", inst->get_unmounted_inst(), CONVERT64TOPRINT(inst->get_id()));
				print_instruction_type(u->inst_type);
				DEBUG_PRINTF("\n");
			#endif
			u->inst = inst;
			u->cycles = 0;
			r = 1;
			break;
		}
	}

/*	for (i=0; i<this->n_units; i++) {
		if (this->units[i].inst == NULL) {
			if (this->units[i].inst_type & t) {
				#ifdef DEBUG_CPU
					DEBUG_PRINTF("\tISSUE INSTRUCTION   %s        id=%u    para func_unit(%u) ", inst->get_unmounted_inst(), CONVERT64TOPRINT(inst->get_id()), i);
					print_instruction_type(this->units[i].inst_type);
					DEBUG_PRINTF("\n");
				#endif
				this->units[i].inst = inst;
				this->units[i].cycles = 0;
				r = 1;
				break;
			}
		}
	}*/

	return r;
}

/*
 Execute instruction one cycle more simulates the pipeline stage write-back
 and prevents the functional unit to be ocupied in the same cycle that the
 instruction left the functional unit
*/

functional_units_response_t* cpu::functional_units_t::exec()
{
	uint32_t i, occupied, vid;
	processor_t *c;

	for (i=0; i<this->get_number_processors_shared(); i++) {
		this->response[i].n_instructions = 0;
	}

	DEBUG_PRINTF("cpu::functional_units_t::exec\n");

	#ifdef DEBUG_CPU
		rs_instruction_t *s;
		for (i=0; i<this->n_units; i++) {
			DEBUG_PRINTF("\tFunctional Unit %u type: ", i);
			print_instruction_type(this->units[i].inst_type);
			if (this->units[i].inst != NULL) {
					s = this->units[i].inst;
					DEBUG_PRINTF("  Instruction (%s) id=%u Remain_cycles=%u\n", s->get_unmounted_inst(), CONVERT64TOPRINT(s->get_id()), s->get_cycles_to_exec()-this->units[i].cycles);
			}
			else {
				DEBUG_PRINTF("  IDLE\n");
			}
		}
	#endif

	occupied = 0;
	for (i=0; i<this->n_units; i++) {
		if (this->units[i].inst != NULL) {
			occupied++;
			c = this->units[i].inst->get_cpu();
			vid = c->get_virtual_id();
			if (this->units[i].cycles++ >= this->units[i].inst->get_cycles_to_exec()) {
				this->response[vid].instructions[ this->response[vid].n_instructions++ ] = this->units[i].inst;
				c->statistics.executed_uops[ this->units[i].inst->get_type() ]++;
				this->units[i].inst = NULL;
			}
		}
	}

	this->get_cpu(0)->statistics.exec_units_cumulative_occupancy += occupied;

	return this->response;
}

void cpu::functional_units_t::flush(processor_t *c)
{
	uint32_t i;

	for (i=0; i<this->n_units; i++) {
		if (this->units[i].inst != NULL) {
			if (this->units[i].inst->get_cpu() == c) {
				this->units[i].inst = NULL;
			}
		}
	}
}

void cpu::functional_units_t::flush_on_write_back(rs_instruction_t *s)
{
	uint32_t i;
	processor_t *c;
	
	c = s->get_cpu();

	for (i=0; i<this->n_units; i++) {
		if (this->units[i].inst != NULL) {
			if (this->units[i].inst->get_cpu() == c && this->units[i].inst->get_id() > s->get_id()) {
				this->units[i].inst = NULL;
			}
		}
	}
}

