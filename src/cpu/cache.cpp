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

    November 2009
*/

#include "cpu.h"
#include "cache.h"

#define cache_debug(...)
//#define cache_debug(...) DEBUG_PRINTF(__VA_ARGS__)

using namespace cpu;

namespace cpu {
	cache_memory_struct_t *cache_memory_struct;
};

void cpu::cache_initialize()
{
	cache_memory_struct = new cache_memory_struct_t();
}

cpu::cache_memory_t::cache_memory_t(uint32_t latency_cycles, uint32_t id)
{
	this->latency_cycles = latency_cycles;
	this->statistic_hits = 0;
	this->statistic_misses = 0;
	this->id = id;
	this->statistic_copy_back = 0;
	
	#ifdef CPU_CACHE_LOCAL_STATISTIC
		this->statistic_local_hits = 0;
		this->statistic_local_misses = 0;
	#endif
}

/***************************************************/

#define GET_INDEX(ADDR) ((ADDR & this->mask_index) >> this->block_bits)
#define GET_TAG(ADDR) (ADDR & this->mask_tag)
#define GET_OFFSET(ADDR) (ADDR & this->mask_offset)
#define GET_BLOCK_ADDR(ADDR) (ADDR & ~this->mask_offset)

cpu::cache_memory_set_associative_t::cache_memory_set_associative_t(uint8_t block_bits, uint32_t set_size, uint8_t index_bits, uint32_t latency_cycles, uint32_t id)
	: cache_memory_t(latency_cycles, id)
{
	uint32_t i, j;
	cache_slot_t *slots;

	this->block_bits = block_bits;
	this->set_size = set_size;
	this->index_bits = index_bits;
	this->block_size = 1 << block_bits;
	this->index_size = 1 << index_bits;

	this->tb = (cache_line_t*)malloc(sizeof(cache_line_t)*this->index_size);
	SANITY_ASSERT(this->tb != NULL);
	for (i=0; i<this->index_size; i++) {
		slots = (cache_slot_t*)malloc(sizeof(cache_slot_t)*this->set_size);
		SANITY_ASSERT(slots != NULL);
		this->tb[i].vector = slots;
		this->tb[i].first = slots;
		this->tb[i].last = slots+(this->set_size - 1);
		for (j=0; j<this->set_size; j++) {
			slots[j].state = CPU_CACHE_SLOT_STATE_EMPTY;
			slots[j].cycle_ready = 0;
			slots[j].next = slots + 1;
		}
		slots[this->set_size - 1].next = NULL;
	}

	this->mask_offset = 0;
	this->mask_index = 0;
	this->mask_tag = 0;

	for (i=0; i<block_bits; i++) {
		this->mask_offset |= 1 << i;
	}

	for (i=block_bits; i<block_bits+index_bits; i++) {
		this->mask_index |= 1 << i;
	}

	this->mask_tag = ~(this->mask_offset | this->mask_index);
}

void cpu::cache_memory_set_associative_t::access(phy_addr_t address, uint8_t length, cache_response_t *response, uint8_t write)
{
	uint32_t i, offset, hit, fake_hit;
	phy_addr_t index, tag, last_address;
	cache_slot_t *line;
	uint64_t last;

	offset = GET_OFFSET(address);
	last_address = address + length - 1;
	response->hit = 1;
	response->blocks = 0;
	response->latency = 0;
	fake_hit = 0;

	while (address <= last_address) {
		index = GET_INDEX(address);
		tag = GET_TAG(address);
		line = this->tb[index].vector;
		hit = 0;
		last = 0;

		for (i=0; i<this->set_size; i++) {
			if (line->state == CPU_CACHE_SLOT_STATE_BUSY && line->tag == tag) {
				hit = 1;
				line->last_access = advancedss_get_system_cycle();
				if (line->cycle_ready > last) {
					last = line->cycle_ready;
				}
				if (write)
					line->dirty = 1;
				break;
			}
			line++;
		}

		if (!hit) {
			response->hit = 0; // if just one block is not on cache, then we don't have a hit
		}
		else {
			response->latency = this->latency_cycles;
			if (last > advancedss_get_system_cycle()) {
				response->latency += last - advancedss_get_system_cycle();
				fake_hit = 1;
			}
		}
		response->blocks++;

		address += this->block_size - offset;
		offset = 0;
	}
	
	if (response->hit && fake_hit == 0) {
		this->statistic_hits++;
		#ifdef CPU_CACHE_LOCAL_STATISTIC
			this->statistic_local_hits++;
		#endif
	}
	else {
		this->statistic_misses++;
		#ifdef CPU_CACHE_LOCAL_STATISTIC
			this->statistic_local_misses++;
		#endif
	}
}

uint32_t cpu::cache_memory_set_associative_t::insert(phy_addr_t address, uint8_t length, uint32_t time_to_be_ready)
{
	uint32_t i, offset;
	phy_addr_t index, tag, last_address;
	cache_slot_t *line, *sub;
	uint32_t latency = 0, block_latency;

	offset = GET_OFFSET(address);
	last_address = address + length - 1;

	while (address <= last_address) {
		index = GET_INDEX(address);
		tag = GET_TAG(address);
		line = this->tb[index].vector;
		sub = NULL;
		block_latency = 0;
		
		for (i=0; i<this->set_size; i++) {
			if (line->state == CPU_CACHE_SLOT_STATE_EMPTY || line->tag == tag) {
				sub = line;
				break;
			}
			line++;
		}

		if (sub == NULL) {
			sub = this->tb[index].vector;
			line = sub+1;
			for (i=1; i<this->set_size; i++) {
				if (line->last_access < sub->last_access) {
					sub = line;
					//break;
				}
				line++;
			}
			if (sub->cycle_ready > advancedss_get_system_cycle())
				block_latency = sub->cycle_ready - advancedss_get_system_cycle();
			if (sub->dirty) {
				latency += cache_memory_struct->copy_back(this, GET_BLOCK_ADDR(address), this->block_size, block_latency);
			}
		}

		sub->tag = tag;
		sub->state = CPU_CACHE_SLOT_STATE_BUSY;
		sub->last_access = advancedss_get_system_cycle();
		sub->dirty = 0;

		sub->cycle_ready = block_latency;
		sub->cycle_ready += advancedss_get_system_cycle() + time_to_be_ready;
		sub->cycle_ready += latency;
//if (sub->cycle_ready > advancedss_get_system_cycle()+500){LOG_PRINTF("lat = %u\n", sub->cycle_ready - advancedss_get_system_cycle());}
		address += this->block_size - offset;
		offset = 0;
	}
	
	return latency+this->latency_cycles;
}

/******************************************************************************/

cpu::cache_memory_struct_t::cache_memory_struct_t()
{
	advancedss_config cfg;
	
	this->caches = (cache_memory_t**)malloc(sizeof(cache_memory_t*)*3);
	SANITY_ASSERT(this->caches != NULL);
	this->links = (cache_memory_t**)malloc(sizeof(cache_memory_t*)*3);
	SANITY_ASSERT(this->links != NULL);
	
	cfg.open("cache.cfg");
	
	this->caches[0] = new cache_memory_set_associative_t(cfg.read_int32("l1_inst_block_bits"), cfg.read_int32("l1_inst_set_size"), cfg.read_int32("l1_inst_index_bits"), cfg.read_int32("l1_inst_latency_cycles"), 0);
	this->caches[1] = new cache_memory_set_associative_t(cfg.read_int32("l1_data_block_bits"), cfg.read_int32("l1_data_set_size"), cfg.read_int32("l1_data_index_bits"), cfg.read_int32("l1_data_latency_cycles"), 1);
	this->caches[2] = new cache_memory_set_associative_t(cfg.read_int32("l2_block_bits"), cfg.read_int32("l2_set_size"), cfg.read_int32("l2_index_bits"), cfg.read_int32("l2_latency_cycles"), 2);
	
	this->memory_penalty = cfg.read_int32("memory_latency_cycles");
	
	this->links[0] = this->caches[2];
	this->links[1] = this->caches[2];
	this->links[2] = NULL;
	
	cache_debug("latencia: %u\n", this->read(0, 0, 10));
	cache_debug("latencia: %u\n", this->read(0, 10, 60));
	cache_debug("latencia: %u\n", this->read(0, 10, 60));
	cache_debug("latencia: %u\n", this->read(0, 75, 10));
	
	this->statistic_memory_latency_medium = 0;
	this->statistic_memory_latency_medium_i = 0;
	
	#ifdef CPU_CACHE_LOCAL_STATISTIC
		this->statistic_local_i = 0;
		this->statistic_local_n = 0;
	#endif
	//exit(0);
}

uint32_t cpu::cache_memory_struct_t::access(uint32_t cache_id, phy_addr_t address, uint8_t length, uint8_t write)
{
	cache_memory_t *l1, *l2;
	cache_response_t r, r2;
	uint32_t latency;
	
	l1 = this->caches[cache_id];
	l2 = this->links[cache_id];
	
	l1->access(address, length, &r, write);
	if (r.hit) {
		latency = r.latency;
		cache_debug("achou na l1\n");
	}
	else {
		l2->access(address, length, &r2, write);
		if (r2.hit) {
			latency = l1->get_latency_cycles() + r2.latency;
			l1->insert(address, length, latency);
			cache_debug("achou na l2\n");
		}
		else {
			latency = l1->get_latency_cycles() + l2->get_latency_cycles() + this->memory_penalty;
			l1->insert(address, length, latency);
			l2->insert(address, length, latency);
			cache_debug("achou na memoria\n");
		}
	}
	
	this->statistic_memory_latency_medium += latency;
	this->statistic_memory_latency_medium_i++;
	
	cache_debug("\tlatency = %u\n", latency);
	if (latency > 500)
		exit(1);
	
	return latency;
}

uint32_t cpu::cache_memory_struct_t::copy_back(cache_memory_t *cache, uint32_t addr, uint32_t len, uint32_t time_to_be_ready)
{
	uint32_t latency;
	cache->statistic_copy_back++;
	if (this->links[cache->get_id()] != NULL)
		latency = this->links[cache->get_id()]->insert(addr, len, time_to_be_ready);
	else
		latency = this->memory_penalty;
	return latency;
}

#ifdef CPU_CACHE_LOCAL_STATISTIC

void cpu::cache_memory_struct_t::event_contex_switch(processor_t *c)
{
	this->statistic_local_i = 1;
	this->statistic_local_hits[0][0] = this->caches[0]->statistic_local_hits;
	this->statistic_local_hits[1][0] = this->caches[1]->statistic_local_hits;
	this->statistic_local_hits[2][0] = this->caches[2]->statistic_local_hits;
	this->statistic_local_misses[0][0] = this->caches[0]->statistic_local_misses;
	this->statistic_local_misses[1][0] = this->caches[1]->statistic_local_misses;
	this->statistic_local_misses[2][0] = this->caches[2]->statistic_local_misses;
	
	this->caches[0]->statistic_local_hits = 0;
	this->caches[0]->statistic_local_misses = 0;
	this->caches[1]->statistic_local_hits = 0;
	this->caches[1]->statistic_local_misses = 0;
	this->caches[2]->statistic_local_hits = 0;
	this->caches[2]->statistic_local_misses = 0;
}

void cpu::cache_memory_struct_t::event_cycle()
{
	if (this->statistic_local_i < CPU_CACHE_LOCAL_STATISTIC) {
		this->statistic_local_hits[0][this->statistic_local_i] = this->caches[0]->statistic_local_hits;
		this->statistic_local_hits[1][this->statistic_local_i] = this->caches[1]->statistic_local_hits;
		this->statistic_local_hits[2][this->statistic_local_i] = this->caches[2]->statistic_local_hits;
		this->statistic_local_misses[0][this->statistic_local_i] = this->caches[0]->statistic_local_misses;
		this->statistic_local_misses[1][this->statistic_local_i] = this->caches[1]->statistic_local_misses;
		this->statistic_local_misses[2][this->statistic_local_i] = this->caches[2]->statistic_local_misses;
		this->statistic_local_i++;
		
		if (this->statistic_local_i == CPU_CACHE_LOCAL_STATISTIC) {
			FILE *fp;
			uint32_t i;
			char fname[100];
			
			sprintf(fname, "statistic/statistic_cache_local_%u_l1_inst.txt", this->statistic_local_n);
			fp = fopen(fname, "w");
			if (fp) {
				for (i=0; i<CPU_CACHE_LOCAL_STATISTIC; i++) {
					fprintf(fp, "%llu-%llu\n", this->statistic_local_hits[0][i], this->statistic_local_misses[0][i]);
				}
			}
			fclose(fp);
			
			sprintf(fname, "statistic/statistic_cache_local_%u_l1_data.txt", this->statistic_local_n);
			fp = fopen(fname, "w");
			if (fp) {
				for (i=0; i<CPU_CACHE_LOCAL_STATISTIC; i++) {
					fprintf(fp, "%llu-%llu\n", this->statistic_local_hits[1][i], this->statistic_local_misses[1][i]);
				}
			}
			fclose(fp);
			
			sprintf(fname, "statistic/statistic_cache_local_%u_l2.txt", this->statistic_local_n);
			fp = fopen(fname, "w");
			if (fp) {
				for (i=0; i<CPU_CACHE_LOCAL_STATISTIC; i++) {
					fprintf(fp, "%llu-%llu\n", this->statistic_local_hits[2][i], this->statistic_local_misses[2][i]);
				}
			}
			fclose(fp);
			
			this->statistic_local_n++;
		}
	}
}

#endif

