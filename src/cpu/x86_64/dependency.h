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

    December 2009
*/

#ifndef __ADVANCEDSS_X8664_DEP_CHECK__H_
#define __ADVANCEDSS_X8664_DEP_CHECK__H_

struct x8664_dep_rs_num_type_t {
	uint8_t num;
	uint32_t types[CPU_DECODER_MAX_ALLOCATED_INST];
	x8664_dep_rs_num_type_t *another;
};

struct x8664_dep_call_table_t {
	void (cpu::decoder_t::*handler)();
	x8664_dep_call_table_t *another;
};

#endif
