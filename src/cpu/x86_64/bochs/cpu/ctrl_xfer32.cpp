/////////////////////////////////////////////////////////////////////////
// $Id: ctrl_xfer32.cc,v 1.85 2009/03/22 21:12:35 sshwarts Exp $
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

#if BX_CPU_LEVEL >= 3

/*BX_CPP_INLINE void BX_CPP_AttrRegparmN(1) BX_CPU_C::branch_near32(Bit32u new_EIP)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}*/

void BX_CPP_AttrRegparmN(1) BX_CPU_C::RETnear32_Iw(bxInstruction_c *i)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::RETnear32(bxInstruction_c *i)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::RETfar32_Iw(bxInstruction_c *i)
{
  BOCHS_MOD_INVALID_INSTRUCTION
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::RETfar32(bxInstruction_c *i)
{
  BOCHS_MOD_INVALID_INSTRUCTION
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::CALL_Jd(bxInstruction_c *i)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::CALL32_Ap(bxInstruction_c *i)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::CALL_EdR(bxInstruction_c *i)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::CALL32_Ep(bxInstruction_c *i)
{
  BOCHS_MOD_INVALID_INSTRUCTION
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JMP_Jd(bxInstruction_c *i)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JO_Jd(bxInstruction_c *i)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JNO_Jd(bxInstruction_c *i)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JB_Jd(bxInstruction_c *i)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JNB_Jd(bxInstruction_c *i)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JZ_Jd(bxInstruction_c *i)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JNZ_Jd(bxInstruction_c *i)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JBE_Jd(bxInstruction_c *i)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JNBE_Jd(bxInstruction_c *i)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JS_Jd(bxInstruction_c *i)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JNS_Jd(bxInstruction_c *i)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JP_Jd(bxInstruction_c *i)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JNP_Jd(bxInstruction_c *i)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JL_Jd(bxInstruction_c *i)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JNL_Jd(bxInstruction_c *i)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JLE_Jd(bxInstruction_c *i)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JNLE_Jd(bxInstruction_c *i)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JMP_Ap(bxInstruction_c *i)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JMP_EdR(bxInstruction_c *i)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}

/* Far indirect jump */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::JMP32_Ep(bxInstruction_c *i)
{
  BOCHS_MOD_INVALID_INSTRUCTION
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::IRET32(bxInstruction_c *i)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JECXZ_Jb(bxInstruction_c *i)
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

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOOPNE32_Jb(bxInstruction_c *i)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOOPE32_Jb(bxInstruction_c *i)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOOP32_Jb(bxInstruction_c *i)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}

#endif
