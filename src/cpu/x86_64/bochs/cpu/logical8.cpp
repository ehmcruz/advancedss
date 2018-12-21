/////////////////////////////////////////////////////////////////////////
// $Id: logical8.cc,v 1.43 2009/01/16 18:18:58 sshwarts Exp $
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

void BX_CPP_AttrRegparmN(1) BX_CPU_C::XOR_EbGbM(bxInstruction_c *i)
{
  Bit8u op1, op2;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op1 = read_RMW_virtual_byte(i->seg(), eaddr);
  op2 = BX_READ_8BIT_REGx(i->nnn(), i->extend8bitL());
  op1 ^= op2;
  write_RMW_virtual_byte(op1);

  SET_FLAGS_OSZAPC_LOGIC_8(op1);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::XOR_GbEbR(bxInstruction_c *i)
{
  Bit8u op1, op2;

  op1 = BX_READ_8BIT_REGx(i->nnn(), i->extend8bitL());
  op2 = BX_READ_8BIT_REGx(i->rm(), i->extend8bitL());
  op1 ^= op2;
  BX_WRITE_8BIT_REGx(i->nnn(), i->extend8bitL(), op1);

  SET_FLAGS_OSZAPC_LOGIC_8(op1);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::XOR_ALIb(bxInstruction_c *i)
{
  Bit8u op1;

  op1 = AL;
  op1 ^= i->Ib();
  AL = op1;

  SET_FLAGS_OSZAPC_LOGIC_8(op1);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::XOR_EbIbM(bxInstruction_c *i)
{
  Bit8u op1, op2 = i->Ib();

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op1 = read_RMW_virtual_byte(i->seg(), eaddr);
  op1 ^= op2;
  write_RMW_virtual_byte(op1);

  SET_FLAGS_OSZAPC_LOGIC_8(op1);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::XOR_EbIbR(bxInstruction_c *i)
{
  Bit8u op1, op2 = i->Ib();

  op1 = BX_READ_8BIT_REGx(i->rm(), i->extend8bitL());
  op1 ^= op2;
  BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), op1);

  SET_FLAGS_OSZAPC_LOGIC_8(op1);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::OR_EbIbM(bxInstruction_c *i)
{
  Bit8u op1, op2 = i->Ib();

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op1 = read_RMW_virtual_byte(i->seg(), eaddr);
  op1 |= op2;
  write_RMW_virtual_byte(op1);

  SET_FLAGS_OSZAPC_LOGIC_8(op1);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::OR_EbIbR(bxInstruction_c *i)
{
  Bit8u op1, op2 = i->Ib();

  op1 = BX_READ_8BIT_REGx(i->rm(), i->extend8bitL());
  op1 |= op2;
  BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), op1);

  SET_FLAGS_OSZAPC_LOGIC_8(op1);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::NOT_EbM(bxInstruction_c *i)
{
  Bit8u op1_8;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op1_8 = read_RMW_virtual_byte(i->seg(), eaddr);
  op1_8 = ~op1_8;
  write_RMW_virtual_byte(op1_8);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::NOT_EbR(bxInstruction_c *i)
{
  Bit8u op1_8 = BX_READ_8BIT_REGx(i->rm(), i->extend8bitL());
  op1_8 = ~op1_8;
  BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), op1_8);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::OR_EbGbM(bxInstruction_c *i)
{
  Bit8u op1, op2;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op1 = read_RMW_virtual_byte(i->seg(), eaddr);
  op2 = BX_READ_8BIT_REGx(i->nnn(), i->extend8bitL());
  op1 |= op2;
  write_RMW_virtual_byte(op1);

  SET_FLAGS_OSZAPC_LOGIC_8(op1);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::OR_GbEbR(bxInstruction_c *i)
{
  Bit8u op1, op2;

  op1 = BX_READ_8BIT_REGx(i->nnn(), i->extend8bitL());
  op2 = BX_READ_8BIT_REGx(i->rm(), i->extend8bitL());
  op1 |= op2;
  BX_WRITE_8BIT_REGx(i->nnn(), i->extend8bitL(), op1);

  SET_FLAGS_OSZAPC_LOGIC_8(op1);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::OR_ALIb(bxInstruction_c *i)
{
  Bit8u op1, op2;

  op1 = AL;
  op2 = i->Ib();
  op1 |= op2;
  AL = op1;

  SET_FLAGS_OSZAPC_LOGIC_8(op1);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::AND_EbGbM(bxInstruction_c *i)
{
  Bit8u op1, op2;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op1 = read_RMW_virtual_byte(i->seg(), eaddr);
  op2 = BX_READ_8BIT_REGx(i->nnn(), i->extend8bitL());
  op1 &= op2;
  write_RMW_virtual_byte(op1);

  SET_FLAGS_OSZAPC_LOGIC_8(op1);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::AND_GbEbR(bxInstruction_c *i)
{
  Bit8u op1, op2;

  op1 = BX_READ_8BIT_REGx(i->nnn(), i->extend8bitL());
  op2 = BX_READ_8BIT_REGx(i->rm(), i->extend8bitL());
  op1 &= op2;
  BX_WRITE_8BIT_REGx(i->nnn(), i->extend8bitL(), op1);

  SET_FLAGS_OSZAPC_LOGIC_8(op1);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::AND_ALIb(bxInstruction_c *i)
{
  Bit8u op1, op2;

  op1 = AL;
  op2 = i->Ib();
  op1 &= op2;
  AL = op1;

  SET_FLAGS_OSZAPC_LOGIC_8(op1);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::AND_EbIbM(bxInstruction_c *i)
{
  Bit8u op1, op2 = i->Ib();

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op1 = read_RMW_virtual_byte(i->seg(), eaddr);
  op1 &= op2;
  write_RMW_virtual_byte(op1);

  SET_FLAGS_OSZAPC_LOGIC_8(op1);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::AND_EbIbR(bxInstruction_c *i)
{
  Bit8u op1, op2 = i->Ib();

  op1 = BX_READ_8BIT_REGx(i->rm(), i->extend8bitL());
  op1 &= op2;
  BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), op1);

  SET_FLAGS_OSZAPC_LOGIC_8(op1);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::TEST_EbGbR(bxInstruction_c *i)
{
  Bit8u op1, op2;

  op1 = BX_READ_8BIT_REGx(i->rm(), i->extend8bitL());
  op2 = BX_READ_8BIT_REGx(i->nnn(), i->extend8bitL());
  op1 &= op2;

  SET_FLAGS_OSZAPC_LOGIC_8(op1);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::TEST_ALIb(bxInstruction_c *i)
{
  Bit8u op1, op2;

  op1 = AL;
  op2 = i->Ib();
  op1 &= op2;

  SET_FLAGS_OSZAPC_LOGIC_8(op1);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::TEST_EbIbR(bxInstruction_c *i)
{
  Bit8u op1 = BX_READ_8BIT_REGx(i->rm(), i->extend8bitL());
  op1 &= i->Ib();
  SET_FLAGS_OSZAPC_LOGIC_8(op1);
}