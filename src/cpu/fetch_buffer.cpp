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
#include <string.h>
#include "cpu.h"

using namespace cpu;

cpu::fetch_buffer_t::fetch_buffer_t(uint32_t number_processors_shared, branch_pred_t *bp)
	: cpu_shared_component_t(number_processors_shared)
{
	advancedss_config *cfg = get_advancedss_main_config();
	uint32_t i;

	this->bp = bp;

	this->size = cfg->read_int32("fetch_buffer_size");
	this->width = cfg->read_int32("fetch_buffer_width");

	this->buffer = (fetch_buffer_el_t**)malloc(number_processors_shared * sizeof(fetch_buffer_el_t*));
	SANITY_ASSERT(this->buffer != NULL);
	this->available = (uint32_t*)malloc(number_processors_shared * sizeof(uint32_t));
	SANITY_ASSERT(this->available != NULL);
	this->available_before = (uint32_t*)malloc(number_processors_shared * sizeof(uint32_t));
	SANITY_ASSERT(this->available_before != NULL);
	this->read_pos = (uint32_t*)malloc(number_processors_shared * sizeof(uint32_t));
	SANITY_ASSERT(this->read_pos != NULL);
	this->write_pos = (uint32_t*)malloc(number_processors_shared * sizeof(uint32_t));
	SANITY_ASSERT(this->write_pos != NULL);
	this->disable_until_flush = (uint8_t*)malloc(number_processors_shared * sizeof(uint8_t));
	SANITY_ASSERT(this->disable_until_flush != NULL);
	this->disable_due_to_cache_miss = (uint32_t*)malloc(number_processors_shared * sizeof(uint32_t));
	SANITY_ASSERT(this->disable_due_to_cache_miss != NULL);
	this->last_branch = (uint32_t*)malloc(number_processors_shared * sizeof(uint32_t));
	SANITY_ASSERT(this->last_branch != NULL);

	for (i=0; i<number_processors_shared; i++) {
		this->buffer[i] = (fetch_buffer_el_t*) malloc (this->size * sizeof(fetch_buffer_el_t));
		if (!this->buffer[i]) {
			LOG_PRINTF("cpu::fetch_buffer_t::fetch_buffer_t malloc error\n");
			exit(1);
		}
		this->available[i] = this->size;
		this->available_before[i] = this->available[i];
		this->read_pos[i] = 0;
		this->write_pos[i] = 0;
		this->disable_until_flush[i] = 0;
		this->disable_due_to_cache_miss[i] = 0;
		this->last_branch[i] = 0;
	}
}

void cpu::fetch_buffer_t::before_decoder()
{
	uint32_t i;
	
	for (i=0; i<this->get_number_processors_shared(); i++) {
		this->available_before[i] = this->available[i];
	}
}

void cpu::fetch_buffer_t::after_decoder()
{
	uint32_t i;
	
	for (i=0; i<this->get_number_processors_shared(); i++) {
		this->get_cpu(i)->statistics.decode_width_used_cumulative += this->available[i] - this->available_before[i];
	}
}

void cpu::fetch_buffer_t::flush(processor_t *c)
{
	uint64_t pc;
	uint32_t vid;

	vid	= c->get_virtual_id();
	this->available[vid] = this->size;
	this->read_pos[vid] = 0;
	this->write_pos[vid] = 0;
	this->disable_until_flush[vid] = 0;
	this->disable_due_to_cache_miss[vid] = 0;
	this->last_branch[vid] = 0;
	this->available_before[vid] = this->available[vid];

	pc = c->get_pc();
	c->set_spec_pc(pc);
	//LOG_PRINTF("spec pc set to 0x"PRINTF_INT64X_PREFIX"\n",pc);
}

uint32_t cpu::fetch_buffer_t::skip_bytes(fetch_buffer_el_t *r, uint32_t n, uint32_t vid) {
	uint32_t skipped, last;
	
	skipped = this->size - this->available[vid]; // skipped now has the number of bytes in buffer
	skipped = (n < skipped) ? n : skipped; // we can't skip more bytes than what we already have
	
	// now, skipped has the number of bytes to be be skkiped
	
	last = (this->read_pos[vid] + skipped - 1) % this->size;
	memcpy(r, this->buffer[vid] + last, sizeof(fetch_buffer_el_t));
	this->read_pos[vid] = (this->read_pos[vid] + skipped) % this->size;
	this->available[vid] += skipped;
	
	return skipped;
}

uint32_t cpu::fetch_buffer_t::skip_bytes(target_addr_t *vaddr, uint32_t n, uint32_t vid)
{
	uint32_t skipped, last;
	
	skipped = this->size - this->available[vid]; // skipped now has the number of bytes in buffer
	skipped = (n < skipped) ? n : skipped; // we can't skip more bytes than what we already have
	
	// now, skipped has the number of bytes to be be skkiped
	
	last = (this->read_pos[vid] + skipped - 1) % this->size;
	*vaddr = (this->buffer[vid] + last)->next_inst;
	this->read_pos[vid] = (this->read_pos[vid] + skipped) % this->size;
	this->available[vid] += skipped;
	
	return skipped;
}

void cpu::fetch_buffer_t::get_byte(fetch_buffer_el_t *r, processor_t *c)
{
	uint32_t vid = c->get_virtual_id();
	memcpy(r, this->buffer[vid] + this->read_pos[vid], sizeof(fetch_buffer_el_t));
	this->read_pos[vid] = (this->read_pos[vid] + 1) % this->size;
	this->available[vid]++;
}

void cpu::fetch_buffer_t::fetch()
{
	uint32_t i;
	for (i=0; i<this->get_number_processors_shared(); i++) {
		this->fetch(i);
	}
}

void cpu::fetch_buffer_t::fetch(uint32_t vid)
{
	uint32_t paddr;
	target_addr_t pc;
	branch_response_t br;
	uint32_t max, i, penalty;
	fetch_buffer_el_t *b;
	uint8_t has_exception;
	exception_code_t exception;
	processor_t *cpu;

	cpu = this->get_cpu(vid);
	max = (this->available[vid] > this->width) ? this->width : this->available[vid];

	pc = cpu->get_spec_pc();

	#ifdef DEBUG_CPU
		DEBUG_PRINTF("buffer:\n");
		for (i=0; i<this->size; i++) {
			if (i == this->write_pos[vid])
				DEBUG_PRINTF("w-");
			if (i == this->read_pos[vid])
				DEBUG_PRINTF("r-");
			DEBUG_PRINTF("%X ", (this->buffer[vid] + i)->data);
		}
		DEBUG_PRINTF("\n");
	#endif

	if (this->disable_until_flush[vid]) {
		DEBUG_PRINTF("fetch disabled until pipeline flush\n");
		return;
	}
	
	if (this->disable_due_to_cache_miss[vid]) {
		DEBUG_PRINTF("fetch disabled due to cache miss for %u cycles\n", this->disable_due_to_cache_miss[vid]);
		this->disable_due_to_cache_miss[vid]--;
		return;
	}

	for (i=0; i<max; i++) {
		b = this->buffer[vid] + this->write_pos[vid];
		paddr = cpu->memory_read_inst_byte(pc, has_exception, exception);
		b->vaddr = pc;
		
		penalty = cache_memory_struct->access(0, paddr, 1, CPU_CACHE_READ);
		if (penalty > 1 && penalty > this->disable_due_to_cache_miss[vid]) //{
			this->disable_due_to_cache_miss[vid] = penalty - 1;
/*			LOG_PRINTF("cache miss penalty = %u\n", penalty);
		}
		else {
			LOG_PRINTF("cache-hit!\n");
		}*/

		this->available[vid]--;
		this->write_pos[vid]++;
		this->write_pos[vid] %= this->size;
		this->get_cpu(vid)->statistics.fetch_width_used_cumulative++;
		this->last_branch[vid]++;

		if (!has_exception) {
			b->is_exception = 0;
			b->paddr = paddr;

			this->bp->check(pc, &br, cpu);
			if (br.is_branch && br.take) {
				b->is_branch = 1;
				b->next_inst = br.target;
				pc = br.target;
				this->get_cpu(vid)->statistics.average_bytes_between_branchs += this->last_branch[vid];
				this->get_cpu(vid)->statistics.average_bytes_between_branchs_i++;
				this->last_branch[vid] = 0;
				break;
			}
			else {
				b->is_branch = 0;
				b->next_inst = ++pc;
			}
		}
		else {
			b->is_exception = 1;
			b->exception_code = exception;
			this->disable_until_flush[vid] = 1;
//LOG_PRINTF("fetch exception %llu\n", advancedss_get_system_cycle());
			break;
		}
	}

	#ifdef DEBUG_CPU
		DEBUG_PRINTF("buffer:\n");
		for (i=0; i<this->size; i++) {
			if (i == this->write_pos[vid])
				DEBUG_PRINTF("w-");
			if (i == this->read_pos[vid])
				DEBUG_PRINTF("r-");
			DEBUG_PRINTF("%X ", (this->buffer[vid] + i)->data);
		}
		DEBUG_PRINTF("\n");
	#endif

	cpu->set_spec_pc(pc);
	this->get_cpu(vid)->statistics.fetch_queue_cumulative_occupancy += (this->size - this->available[vid]);
}
