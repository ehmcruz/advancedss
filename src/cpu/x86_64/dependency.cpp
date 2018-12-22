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

#include "../cpu.h"

using namespace cpu;

/**********************************************/

#define NO_LOOKUP(IDENT, N, TYPES)       { N, TYPES, NULL },
#define INVALID                 { 1, {CPU_INSTRUCTION_TYPE_NONE}, NULL },
#define NO_LOOKUP_(IDENT, N, TYPES)      { N, TYPES, NULL },
#define LOOKUP(TB)              { 0, {}, TB},

#define LIST_TYPES(...)         { __VA_ARGS__ }

extern const x8664_dep_rs_num_type_t x8664_rs_num_type[512*2+1] = {
  // 512 entries for without memory + 512 entries with memory
  #include "dep.h"
  NULL
};

#undef NO_LOOKUP
#undef NO_LOOKUP_
#undef LOOKUP
#undef INVALID
#undef LIST_TYPES

/**********************************************/

#define INVALID                 { &decoder_t::implement_inst_invalid, NULL },
#define LOOKUP(TB)              { NULL, TB },

#define LIST_TYPES(...)

extern const x8664_dep_call_table_t x8664_dep_call_tb[512*2*3+1] = {
  // 512 entries for without memory + 512 entries with memory
  // opsize = 16 bits
  #define NO_LOOKUP(IDENT, N, TYPES)       { &decoder_t::implement_inst_16bits_##IDENT, NULL },
  #define NO_LOOKUP_(IDENT, N, TYPES)      NO_LOOKUP(IDENT, N, TYPES)
  #include "dep.h"
  #undef NO_LOOKUP_
  #undef NO_LOOKUP

  // 512 entries for without memory + 512 entries with memory
  // opsize = 32 bits
  #define NO_LOOKUP(IDENT, N, TYPES)       { &decoder_t::implement_inst_32bits_##IDENT, NULL },
  #define NO_LOOKUP_(IDENT, N, TYPES)      NO_LOOKUP(IDENT, N, TYPES)
  #include "dep.h"
  #undef NO_LOOKUP_
  #undef NO_LOOKUP

  // 512 entries for without memory + 512 entries with memory
  // opsize = 64 bits
  #define NO_LOOKUP(IDENT, N, TYPES)       { &decoder_t::implement_inst_64bits_##IDENT, NULL },
  #define NO_LOOKUP_(IDENT, N, TYPES)      NO_LOOKUP(IDENT, N, TYPES)
  #include "dep.h"
  #undef NO_LOOKUP_
  #undef NO_LOOKUP

  NULL
};

#undef LOOKUP
#undef INVALID
#undef LIST_TYPES

/**********************************************/

void cpu::decoder_t::implement_inst_invalid()
{
	this->statistic_instructions_invalid++;
}

void cpu::decoder_t::implement_inst_16bits_invalid()
{
	this->statistic_instructions_invalid++;
}

void cpu::decoder_t::implement_inst_32bits_invalid()
{
	this->statistic_instructions_invalid++;
}

void cpu::decoder_t::implement_inst_64bits_invalid()
{
	this->statistic_instructions_invalid++;
}

#include "../decoder_cpp.h"
#include "decoder_utils.h"
#include "dep2.h"
#include "dep3.h"
#include "dep4.h"
#include "dep5.h"

#define NO_LOOKUP(IDENT, N, TYPES) \
	void cpu::decoder_t::implement_inst_16bits_##IDENT () \
	{ \
		IDENT##_HEAD \
		IDENT##_16BITS \
		IDENT##_TAIL \
	} \
	void cpu::decoder_t::implement_inst_32bits_##IDENT () \
	{ \
		IDENT##_HEAD \
		IDENT##_32BITS \
		IDENT##_TAIL \
	} \
	void cpu::decoder_t::implement_inst_64bits_##IDENT () \
	{ \
		IDENT##_HEAD \
		IDENT##_64BITS \
		IDENT##_TAIL \
	}

#define INVALID
#define NO_LOOKUP_(IDENT, N, TYPES)
#define LOOKUP(TB)
#define LIST_TYPES(...)

#include "dep.h"

#undef NO_LOOKUP_
#undef LOOKUP
#undef INVALID
#undef LIST_TYPES

