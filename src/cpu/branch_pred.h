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

#ifndef _ADVANCEDSS_BRANCHPRED_HEADER_
#define _ADVANCEDSS_BRANCHPRED_HEADER_

#include "cpu.h"
#include "../data/double_linked_static.h"

namespace cpu
{
	class branch_pred_take_none_t: public branch_pred_t
	{
		public:
			branch_pred_take_none_t(uint32_t number_processors_shared);

			void add_entry(branch_el_t *branch, processor_t *proc);
			void check (target_addr_t vaddr, branch_response_t *r, processor_t *proc);
			void flush(processor_t *proc);
	};
	
	/*********************************************************************************/
	
	struct branch_pred_take_all_el_t {
		target_addr_t inst_addr;
		target_addr_t target;
		processor_t *proc;
		int8_t count;
	};
	
	typedef data_dblinked_static_list<branch_pred_take_all_el_t, target_addr_t> branch_pred_take_all_list_t;
	typedef data_dblinked_static_el<branch_pred_take_all_el_t, target_addr_t> branch_pred_take_all_list_el_t;
	
	class branch_pred_take_all_t: public branch_pred_t
	{
		protected:
			branch_pred_take_all_list_t list, list_supplier;
			branch_pred_take_all_list_el_t **hash;
			uint32_t pt; // power of two
			target_addr_t mask;
			
		public:
			branch_pred_take_all_t(uint32_t number_processors_shared);

			void add_entry(branch_el_t *branch, processor_t *proc);
			void check (target_addr_t vaddr, branch_response_t *r, processor_t *proc);
			void flush(processor_t *proc);
			branch_pred_take_all_list_el_t* search(target_addr_t addr, processor_t *c);
	};
	
	class branch_pred_hist_tb_t: public branch_pred_t
	{
		protected:
			branch_pred_take_all_list_t list, list_supplier;
			branch_pred_take_all_list_el_t **hash;
			uint32_t pt; // power of two
			target_addr_t mask;
			
		public:
			branch_pred_hist_tb_t(uint32_t number_processors_shared);

			void add_entry(branch_el_t *branch, processor_t *proc);
			void check (target_addr_t vaddr, branch_response_t *r, processor_t *proc);
			void flush(processor_t *proc);
			branch_pred_take_all_list_el_t* search(target_addr_t addr, processor_t *c);
	};
};

#endif
