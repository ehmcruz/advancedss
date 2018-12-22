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
#include "branch_pred.h"

using namespace cpu;

cpu::branch_pred_t::branch_pred_t(uint32_t number_processors_shared)
	: cpu_shared_component_t(number_processors_shared)
{
	this->statistic_hash_hit = 0;
	this->statistic_hash_miss = 0;
}

/**********************************************************/

cpu::branch_pred_take_none_t::branch_pred_take_none_t(uint32_t number_processors_shared)
    : branch_pred_t(number_processors_shared)
{
	DEBUG_PRINTF("Branch prediction none\n");
}

void cpu::branch_pred_take_none_t::add_entry(branch_el_t *branch, processor_t *proc)
{
}

void cpu::branch_pred_take_none_t::check (target_addr_t vaddr, branch_response_t *r, processor_t *proc)
{
	r->is_branch = 0;
}

void cpu::branch_pred_take_none_t::flush(processor_t *proc)
{
}

/**********************************************************/

cpu::branch_pred_take_all_t::branch_pred_take_all_t(uint32_t number_processors_shared)
    : branch_pred_t(number_processors_shared)
{
	advancedss_config *cfg = get_advancedss_main_config();
	uint32_t size, i;
	branch_pred_take_all_el_t *el;

	size = cfg->read_int32("branch_pred_table_size");
	
	this->pt = 1;
	while (this->pt < size) {
		this->pt <<= 1;
	}
	this->pt <<= 1;
	this->mask = this->pt - 1;
	this->hash = (branch_pred_take_all_list_el_t**)malloc(sizeof(branch_pred_take_all_list_el_t*)*this->pt);
	SANITY_ASSERT(this->hash != NULL);
	
	for (i=0; i<this->pt; i++) {
		this->hash[i] = NULL;
	}

	this->list.set_list_size(size);
	this->list_supplier.set_list_size(size);

	el = (branch_pred_take_all_el_t*)malloc( sizeof(branch_pred_take_all_el_t) * size );
	if (el == NULL) {
		LOG_PRINTF("error: cpu::branch_pred_take_all_t::branch_pred_take_all_t  allocating memory\n");
		exit(1);
	}

	for (i=0; i<size; i++) {
		this->list_supplier.add_end(el + i);
	}

	DEBUG_PRINTF("Branch prediction table size is %u\n", size);
}

void cpu::branch_pred_take_all_t::add_entry(branch_el_t *branch, processor_t *proc)
{
	branch_pred_take_all_list_el_t *el;
	branch_pred_take_all_el_t *data;

	el = this->search( branch->mem_addr, proc );

	if (el) {
		el = this->list.move_to_init(el);
		data = el->get_data();
		DEBUG_PRINTF("Branch Element already in table\n");
	}
	else {
		DEBUG_PRINTF("Branch Element not in table\n");
		if (this->list_supplier.get_n_elements() > 0) {
			el = this->list_supplier.get_first();
			data = el->get_data();
			this->list_supplier.del_first();
			DEBUG_PRINTF("Branch Element allocated from supply\n");
		}
		else {
			el = this->list.get_last();
			data = el->get_data();
			this->list.del_last();
			DEBUG_PRINTF("Branch Element substituted an element\n");
		}
		el = this->list.add_beginning(data);
	}

	data->inst_addr = branch->mem_addr;
	data->target = branch->target_addr;
	data->proc = proc;

	this->hash[branch->mem_addr & this->mask] = el;
}

void cpu::branch_pred_take_all_t::check (target_addr_t vaddr, branch_response_t *r, processor_t *proc)
{
	branch_pred_take_all_list_el_t *el;

	el = this->search( vaddr, proc );

	if (el) {
		r->is_branch = 1;
		r->take = 1;
		r->target = el->get_data()->target;
		DEBUG_PRINTF("Branch check is branch 0x"PRINTF_INT64X_PREFIX" target 0x"PRINTF_INT64X_PREFIX"\n", vaddr, r->target);
	}
	else {
		r->is_branch = 0;
		DEBUG_PRINTF("Branch check is NOT branch 0x"PRINTF_INT64X_PREFIX"\n", vaddr);
	}
}

void cpu::branch_pred_take_all_t::flush(processor_t *proc)
{
	branch_pred_take_all_list_el_t *el;
	branch_pred_take_all_el_t *data;
	uint32_t i;

	for (i=0; i<this->pt; i++) {
		this->hash[i] = NULL;
	}

	for (el=this->list.get_first(); el!=NULL; el=el->get_next()) {
		data = el->get_data();

		if (data->proc == proc) {
			this->list_supplier.add_end(data);
			this->list.del(el);
		}
	}
}

branch_pred_take_all_list_el_t* cpu::branch_pred_take_all_t::search(target_addr_t addr, processor_t *c)
{
	branch_pred_take_all_list_el_t *el;
	branch_pred_take_all_el_t *data;
	branch_pred_take_all_el_t *h;
	
	el = this->hash[addr & this->mask];
	
	if (el != NULL) {
		h = el->get_data();
		if (h->proc == c && h->inst_addr == addr) {
			this->statistic_hash_hit++;
			return el;
		}
	}

	for (el=this->list.get_first(); el!=NULL; el=el->get_next()) {
		data = el->get_data();

		if (data->proc == c && data->inst_addr == addr) {
			this->statistic_hash_miss++;
			return el;
		}
	}

	return NULL;
}
/**********************************************************/

cpu::branch_pred_hist_tb_t::branch_pred_hist_tb_t(uint32_t number_processors_shared)
    : branch_pred_t(number_processors_shared)
{
	advancedss_config *cfg = get_advancedss_main_config();
	uint32_t size, i;
	branch_pred_take_all_el_t *el;

	size = cfg->read_int32("branch_pred_table_size");
	
	this->pt = 1;
	while (this->pt < size) {
		this->pt <<= 1;
	}
	this->pt <<= 1;
	this->mask = this->pt - 1;
	this->hash = (branch_pred_take_all_list_el_t**)malloc(sizeof(branch_pred_take_all_list_el_t*)*this->pt);
	SANITY_ASSERT(this->hash != NULL);
	
	for (i=0; i<this->pt; i++) {
		this->hash[i] = NULL;
	}

	this->list.set_list_size(size);
	this->list_supplier.set_list_size(size);

	el = (branch_pred_take_all_el_t*)malloc( sizeof(branch_pred_take_all_el_t) * size );
	if (el == NULL) {
		LOG_PRINTF("error: cpu::branch_pred_hist_tb_t::branch_pred_hist_tb_t  allocating memory\n");
		exit(1);
	}

	for (i=0; i<size; i++) {
		this->list_supplier.add_end(el + i);
	}

	DEBUG_PRINTF("Branch prediction table size is %u\n", size);
}

void cpu::branch_pred_hist_tb_t::add_entry(branch_el_t *branch, processor_t *proc)
{
	branch_pred_take_all_list_el_t *el;
	branch_pred_take_all_el_t *data;

	el = this->search( branch->mem_addr, proc );

	if (el) {
		el = this->list.move_to_init(el);
		data = el->get_data();
		DEBUG_PRINTF("Branch Element already in table\n");
		data->count = (branch->taken) ? (data->count + 1) : (data->count - 1);
		if (data->count > 1)
			data->count = 1;
		else if (data->count < -1)
			data->count = -1;
	}
	else {
		DEBUG_PRINTF("Branch Element not in table\n");
		if (this->list_supplier.get_n_elements() > 0) {
			el = this->list_supplier.get_first();
			data = el->get_data();
			this->list_supplier.del_first();
			DEBUG_PRINTF("Branch Element allocated from supply\n");
		}
		else {
			el = this->list.get_last();
			data = el->get_data();
			this->list.del_last();
			DEBUG_PRINTF("Branch Element substituted an element\n");
		}
		el = this->list.add_beginning(data);
		data->count = (branch->taken) ? 0 : -1;
	}

	data->inst_addr = branch->mem_addr;
	data->target = branch->target_addr;
	data->proc = proc;

	this->hash[branch->mem_addr & this->mask] = el;
}

void cpu::branch_pred_hist_tb_t::check (target_addr_t vaddr, branch_response_t *r, processor_t *proc)
{
	branch_pred_take_all_list_el_t *el;

	el = this->search( vaddr, proc );

	if (el) {
		r->is_branch = 1;
		r->take = (el->get_data()->count >= 0);
		r->target = el->get_data()->target;
		DEBUG_PRINTF("Branch check is branch 0x"PRINTF_INT64X_PREFIX" target 0x"PRINTF_INT64X_PREFIX"\n", vaddr, r->target);
	}
	else {
		r->is_branch = 0;
		DEBUG_PRINTF("Branch check is NOT branch 0x"PRINTF_INT64X_PREFIX"\n", vaddr);
	}
}

void cpu::branch_pred_hist_tb_t::flush(processor_t *proc)
{
	branch_pred_take_all_list_el_t *el;
	branch_pred_take_all_el_t *data;
	uint32_t i;

	for (i=0; i<this->pt; i++) {
		this->hash[i] = NULL;
	}

	for (el=this->list.get_first(); el!=NULL; el=el->get_next()) {
		data = el->get_data();

		if (data->proc == proc) {
			this->list_supplier.add_end(data);
			this->list.del(el);
		}
	}
}

branch_pred_take_all_list_el_t* cpu::branch_pred_hist_tb_t::search(target_addr_t addr, processor_t *c)
{
	branch_pred_take_all_list_el_t *el;
	branch_pred_take_all_el_t *data;
	branch_pred_take_all_el_t *h;
	
	el = this->hash[addr & this->mask];
	
	if (el != NULL) {
		h = el->get_data();
		if (h->proc == c && h->inst_addr == addr) {
			this->statistic_hash_hit++;
			return el;
		}
	}

	for (el=this->list.get_first(); el!=NULL; el=el->get_next()) {
		data = el->get_data();

		if (data->proc == c && data->inst_addr == addr) {
			this->statistic_hash_miss++;
			return el;
		}
	}

	return NULL;
}
