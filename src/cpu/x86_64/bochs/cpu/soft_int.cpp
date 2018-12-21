/////////////////////////////////////////////////////////////////////////
// $Id: soft_int.cc,v 1.52 2009/04/14 17:41:58 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001  MandrakeSoft S.A.
//
//    MandrakeSoft S.A.
//    43, rue d'Aboukir
//    75002 Paris - France
//    http://www.linux-mandrake.com/
//    http://www.mandrakesoft.com/
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

// Make code more tidy with a few macros.
#if BX_SUPPORT_X86_64==0
#define RSP ESP
#endif

void BX_CPP_AttrRegparmN(1) BX_CPU_C::BOUND_GwMa(bxInstruction_c *i)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::BOUND_GdMa(bxInstruction_c *i)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::INT1(bxInstruction_c *i)
{
  BOCHS_SET_RESULT_TYPE_TRAP(1)
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::INT3(bxInstruction_c *i)
{
  BOCHS_SET_RESULT_TYPE_TRAP(3)
}


void BX_CPP_AttrRegparmN(1) BX_CPU_C::INT_Ib(bxInstruction_c *i)
{
	uint8_t code = i->Ib();
//  RSP_SPECULATIVE;
	BOCHS_SET_RESULT_TYPE_TRAP(code)
//  RSP_COMMIT;
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::INTO(bxInstruction_c *i)
{
  if (get_OF()) {
    BOCHS_SET_RESULT_TYPE_TRAP(4)
  }
}
