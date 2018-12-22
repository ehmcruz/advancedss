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

/*
	This file contains a lot of classes inherited from class cache_memory_t
*/

#ifndef _ADVANCEDSS_CACHE_HEADER_
#define _ADVANCEDSS_CACHE_HEADER_

#include "cpu.h"

namespace cpu {
	enum cache_slot_state_t {
		CPU_CACHE_SLOT_STATE_EMPTY,
		CPU_CACHE_SLOT_STATE_BUSY
	};

	struct cache_slot_t: public cache_pre_slot_t {
		phy_addr_t tag;
		cache_slot_state_t state;
		uint64_t last_access;
		uint8_t dirty;

		cache_slot_t *next;
	};

	struct cache_line_t {
		cache_slot_t *first, *last, *vector;
	};

	class cache_memory_set_associative_t: public cache_memory_t
	{
		protected:
			uint32_t block_bits, block_size;
			uint32_t set_size;
			uint32_t index_bits, index_size;
			phy_addr_t mask_offset, mask_index, mask_tag;
			cache_line_t *tb;


		public:
			cache_memory_set_associative_t(uint8_t block_bits, uint32_t set_size, uint8_t index_bits, uint32_t latency_cycles, uint32_t id);
			void access(phy_addr_t address, uint8_t length, cache_response_t *response, uint8_t write);
			uint32_t insert(phy_addr_t address, uint8_t length, uint32_t time_to_be_ready);
	};
};

#endif
