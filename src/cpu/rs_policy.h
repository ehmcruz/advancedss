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

/*
	This file contains a lot of classes inherited from class rs_policy_t
*/

#ifndef _ADVANCEDSS_RSPOLICY_HEADER_
#define _ADVANCEDSS_RSPOLICY_HEADER_

#include "cpu.h"

namespace cpu {
	class rs_policy_just_one_t: public rs_policy_t
	{
		protected:
			rs_instruction_static_list_t list;

		public:
			rs_policy_just_one_t(functional_units_t *f_units, uint32_t number_processors_shared, uint32_t size);

			uint8_t add(rs_instruction_t *s);
			uint8_t del(rs_instruction_t *s);
			virtual void supply();
			virtual void flush(processor_t *c);
			virtual void flush_on_write_back(rs_instruction_t *s);
			uint32_t get_capacity();
			virtual void arrived_to_commit(rs_instruction_t *s);
	};

	class rs_policy_io_queue_t: public rs_policy_just_one_t
	{
		protected:
			uint8_t *must_issue; // the size depends on the number of shared processors

		public:
			rs_policy_io_queue_t(functional_units_t *f_units, uint32_t number_processors_shared, uint32_t size);

			void supply();
			void arrived_to_commit(rs_instruction_t *s);
			void flush(processor_t *c);
			void flush_on_write_back(rs_instruction_t *s);
	};

	class rs_policy_ls_queue_simple_t: public rs_policy_just_one_t
	{
		protected:
			uint8_t *write_ready; // the size depends on the number of shared processors

		public:
			rs_policy_ls_queue_simple_t(functional_units_t *f_units, uint32_t number_processors_shared, uint32_t size);

			void supply();
			void arrived_to_commit(rs_instruction_t *s);
			void flush(processor_t *c);
			void flush_on_write_back(rs_instruction_t *s);
	};
};

#endif
