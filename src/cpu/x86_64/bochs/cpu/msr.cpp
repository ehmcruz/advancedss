/////////////////////////////////////////////////////////////////////////
// $Id: msr.cc,v 1.17.2.1 2009/06/07 07:49:10 vruppert Exp $
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2008 Stanislav Shwartsman
//          Written by Stanislav Shwartsman [sshwarts at sourceforge net]
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA B 02110-1301 USA
//
/////////////////////////////////////////////////////////////////////////

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

    August 2009
*/

#define NEED_CPU_REG_SHORTCUTS 1
#include "../bochs.h"
#include "../bochs_fake.h"
#include "../../cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

using namespace cpu;

#if BX_SUPPORT_X86_64==0
// Make life easier for merging code.
#define RAX EAX
#define RDX EDX
#endif

void BX_CPP_AttrRegparmN(1) BX_CPU_C::RDMSR(bxInstruction_c *i)
{
	uint32_t code, value_low, value_high;
	uint64_t value;

	CPU_X8664_CHECK_KERNEL_MODE

	code = ECX;
	value = CPU_X8664_READ_MSR(code);
	value_low = value;
	value_high = value >> 32;

	RAX = value_low;
	RDX = value_high;
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::WRMSR(bxInstruction_c *i)
{
	uint32_t code, value_low, value_high;
	uint64_t value;

	CPU_X8664_CHECK_KERNEL_MODE

	code = ECX;
	value_low = EAX;
	value_high = EDX;
	value = (uint64_t)value_low | ((uint64_t)value_high << 32);

	CPU_X8664_WRITE_MSR(code, value)
}
