/////////////////////////////////////////////////////////////////////////
// $Id: ctrl_xfer16.cc,v 1.68 2009/03/22 21:12:35 sshwarts Exp $
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
#include "../../cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

using namespace cpu;

// Make code more tidy with a few macros.
#if BX_SUPPORT_X86_64==0
#define RSP ESP
#define RIP EIP
#endif

/*BX_CPP_INLINE void BX_CPP_AttrRegparmN(1) BX_CPU_C::branch_near16(Bit16u new_IP)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}*/

void BX_CPP_AttrRegparmN(1) BX_CPU_C::RETnear16_Iw(bxInstruction_c *i)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::RETnear16(bxInstruction_c *i)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::RETfar16_Iw(bxInstruction_c *i)
{
  BOCHS_MOD_INVALID_INSTRUCTION
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::RETfar16(bxInstruction_c *i)
{
  BOCHS_MOD_INVALID_INSTRUCTION
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::CALL_Jw(bxInstruction_c *i)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::CALL16_Ap(bxInstruction_c *i)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::CALL_EwR(bxInstruction_c *i)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::CALL16_Ep(bxInstruction_c *i)
{
  BOCHS_MOD_INVALID_INSTRUCTION
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JMP_Jw(bxInstruction_c *i)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JO_Jw(bxInstruction_c *i)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JNO_Jw(bxInstruction_c *i)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JB_Jw(bxInstruction_c *i)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JNB_Jw(bxInstruction_c *i)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JZ_Jw(bxInstruction_c *i)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JNZ_Jw(bxInstruction_c *i)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JBE_Jw(bxInstruction_c *i)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JNBE_Jw(bxInstruction_c *i)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JS_Jw(bxInstruction_c *i)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JNS_Jw(bxInstruction_c *i)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JP_Jw(bxInstruction_c *i)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JNP_Jw(bxInstruction_c *i)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JL_Jw(bxInstruction_c *i)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JNL_Jw(bxInstruction_c *i)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JLE_Jw(bxInstruction_c *i)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JNLE_Jw(bxInstruction_c *i)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JMP_EwR(bxInstruction_c *i)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}

/* Far indirect jump */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::JMP16_Ep(bxInstruction_c *i)
{
  BOCHS_MOD_INVALID_INSTRUCTION
}

/*
	in advancedss, we will only accept the 64 bit variant of iret
*/

void BX_CPP_AttrRegparmN(1) BX_CPU_C::IRET16(bxInstruction_c *i)
{
  BOCHS_MOD_INVALID_INSTRUCTION
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JCXZ_Jb(bxInstruction_c *i)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}

//
// There is some weirdness in LOOP instructions definition. If an exception
// was generated during the instruction execution (for example #GP fault
// because EIP was beyond CS segment limits) CPU state should restore the
// state prior to instruction execution.
//
// The final point that we are not allowed to decrement ECX register before
// it is known that no exceptions can happen.
//

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOOPNE16_Jb(bxInstruction_c *i)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOOPE16_Jb(bxInstruction_c *i)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOOP16_Jb(bxInstruction_c *i)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}
