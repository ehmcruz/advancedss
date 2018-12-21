/////////////////////////////////////////////////////////////////////////
// $Id: logical32.cc,v 1.44 2009/01/16 18:18:58 sshwarts Exp $
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

    Oriented by Ronaldo Augusto de Lara Gon�alves
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

#if BX_SUPPORT_X86_64==0
// Make life easier for merging cpu64 and cpu32 code.
#define RAX EAX
#endif

void BX_CPP_AttrRegparmN(1) BX_CPU_C::XOR_EdGdM(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op1_32 = read_RMW_virtual_dword(i->seg(), eaddr);
  op2_32 = BX_READ_32BIT_REG(i->nnn());
  op1_32 ^= op2_32;
  write_RMW_virtual_dword(op1_32);

  SET_FLAGS_OSZAPC_LOGIC_32(op1_32);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::XOR_GdEdR(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32;

  op1_32 = BX_READ_32BIT_REG(i->nnn());
  op2_32 = BX_READ_32BIT_REG(i->rm());
  op1_32 ^= op2_32;
  BX_WRITE_32BIT_REGZ(i->nnn(), op1_32);

  SET_FLAGS_OSZAPC_LOGIC_32(op1_32);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::XOR_EAXId(bxInstruction_c *i)
{
  Bit32u op1_32;

  op1_32 = EAX;
  op1_32 ^= i->Id();
  RAX = op1_32;

  SET_FLAGS_OSZAPC_LOGIC_32(op1_32);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::XOR_EdIdM(bxInstruction_c *i)
{
  Bit32u op1_32;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op1_32 = read_RMW_virtual_dword(i->seg(), eaddr);
  op1_32 ^= i->Id();
  write_RMW_virtual_dword(op1_32);

  SET_FLAGS_OSZAPC_LOGIC_32(op1_32);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::XOR_EdIdR(bxInstruction_c *i)
{
  Bit32u op1_32 = BX_READ_32BIT_REG(i->rm());
  op1_32 ^= i->Id();
  BX_WRITE_32BIT_REGZ(i->rm(), op1_32);

  SET_FLAGS_OSZAPC_LOGIC_32(op1_32);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::OR_EdIdM(bxInstruction_c *i)
{
  Bit32u op1_32;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op1_32 = read_RMW_virtual_dword(i->seg(), eaddr);
  op1_32 |= i->Id();
  write_RMW_virtual_dword(op1_32);

  SET_FLAGS_OSZAPC_LOGIC_32(op1_32);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::OR_EdIdR(bxInstruction_c *i)
{
  Bit32u op1_32 = BX_READ_32BIT_REG(i->rm());
  op1_32 |= i->Id();
  BX_WRITE_32BIT_REGZ(i->rm(), op1_32);

  SET_FLAGS_OSZAPC_LOGIC_32(op1_32);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::NOT_EdM(bxInstruction_c *i)
{
  Bit32u op1_32;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op1_32 = read_RMW_virtual_dword(i->seg(), eaddr);
  op1_32 = ~op1_32;
  write_RMW_virtual_dword(op1_32);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::NOT_EdR(bxInstruction_c *i)
{
  Bit32u op1_32 = BX_READ_32BIT_REG(i->rm());
  op1_32 = ~op1_32;
  BX_WRITE_32BIT_REGZ(i->rm(), op1_32);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::OR_EdGdM(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op1_32 = read_RMW_virtual_dword(i->seg(), eaddr);
  op2_32 = BX_READ_32BIT_REG(i->nnn());
  op1_32 |= op2_32;
  write_RMW_virtual_dword(op1_32);

  SET_FLAGS_OSZAPC_LOGIC_32(op1_32);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::OR_GdEdR(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32;

  op1_32 = BX_READ_32BIT_REG(i->nnn());
  op2_32 = BX_READ_32BIT_REG(i->rm());
  op1_32 |= op2_32;
  BX_WRITE_32BIT_REGZ(i->nnn(), op1_32);

  SET_FLAGS_OSZAPC_LOGIC_32(op1_32);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::OR_EAXId(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32;

  op1_32 = EAX;
  op2_32 = i->Id();
  op1_32 |= op2_32;
  RAX = op1_32;

  SET_FLAGS_OSZAPC_LOGIC_32(op1_32);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::AND_EdGdM(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op1_32 = read_RMW_virtual_dword(i->seg(), eaddr);
  op2_32 = BX_READ_32BIT_REG(i->nnn());
  op1_32 &= op2_32;
  write_RMW_virtual_dword(op1_32);

  SET_FLAGS_OSZAPC_LOGIC_32(op1_32);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::AND_GdEdR(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32;

  op1_32 = BX_READ_32BIT_REG(i->nnn());
  op2_32 = BX_READ_32BIT_REG(i->rm());
  op1_32 &= op2_32;
  BX_WRITE_32BIT_REGZ(i->nnn(), op1_32);

  SET_FLAGS_OSZAPC_LOGIC_32(op1_32);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::AND_EAXId(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32;

  op1_32 = EAX;
  op2_32 = i->Id();
  op1_32 &= op2_32;
  RAX = op1_32;

  SET_FLAGS_OSZAPC_LOGIC_32(op1_32);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::AND_EdIdM(bxInstruction_c *i)
{
  Bit32u op1_32;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op1_32 = read_RMW_virtual_dword(i->seg(), eaddr);
  op1_32 &= i->Id();
  write_RMW_virtual_dword(op1_32);

  SET_FLAGS_OSZAPC_LOGIC_32(op1_32);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::AND_EdIdR(bxInstruction_c *i)
{
  Bit32u op1_32 = BX_READ_32BIT_REG(i->rm());
  op1_32 &= i->Id();
  BX_WRITE_32BIT_REGZ(i->rm(), op1_32);

  SET_FLAGS_OSZAPC_LOGIC_32(op1_32);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::TEST_EdGdR(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32;

  op1_32 = BX_READ_32BIT_REG(i->rm());
  op2_32 = BX_READ_32BIT_REG(i->nnn());
  op1_32 &= op2_32;

  SET_FLAGS_OSZAPC_LOGIC_32(op1_32);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::TEST_EAXId(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32;

  op1_32 = EAX;
  op2_32 = i->Id();
  op1_32 &= op2_32;

  SET_FLAGS_OSZAPC_LOGIC_32(op1_32);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::TEST_EdIdR(bxInstruction_c *i)
{
  Bit32u op1_32 = BX_READ_32BIT_REG(i->rm());
  op1_32 &= i->Id();
  SET_FLAGS_OSZAPC_LOGIC_32(op1_32);
}
