/////////////////////////////////////////////////////////////////////////
// $Id: v 1.289.2.1 2009/06/07 07:49:10 vruppert Exp $
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

// modified to behave as advancedss expects

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

#define TEST_FOR_ANOTHER_ITERATION

/*
bochs uses this as a iteration test:

	if (BX_CPU_THIS_PTR async_event)
		break; // exit always if debugger enabled

and returns the rip to prev rip in case of break occurs
*/

using namespace cpu;

void BX_CPP_AttrRegparmN(2) BX_CPU_C::repeat(bxInstruction_c *i, BxExecutePtr_tR execute)
{
  // non repeated instruction
  if (! i->repUsedL()) {
    BX_CPU_CALL_METHOD(execute, (i));
    return;
  }

  if (i->as64L()) {
    while(1) {
      if (RCX != 0) {
        BX_CPU_CALL_METHOD(execute, (i));
        BX_INSTR_REPEAT_ITERATION(BX_CPU_ID, i);
        RCX --;
      }
      if (RCX == 0) return;

      TEST_FOR_ANOTHER_ITERATION
    }
  }
  else
  if (i->as32L()) {
    while(1) {
      if (ECX != 0) {
        BX_CPU_CALL_METHOD(execute, (i));
        BX_INSTR_REPEAT_ITERATION(BX_CPU_ID, i);
        RCX = ECX - 1;
      }
      if (ECX == 0) return;

      TEST_FOR_ANOTHER_ITERATION
    }
  }
  else  // 16bit addrsize
  {
    BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
  }
}

void BX_CPP_AttrRegparmN(2) BX_CPU_C::repeat_ZF(bxInstruction_c *i, BxExecutePtr_tR execute)
{
  unsigned rep = i->repUsedValue();

  // non repeated instruction
  if (! rep) {
    BX_CPU_CALL_METHOD(execute, (i));
    return;
  }

  if (rep == 3) { /* repeat prefix 0xF3 */
    if (i->as64L()) {
      while(1) {
        if (RCX != 0) {
          BX_CPU_CALL_METHOD(execute, (i));
          BX_INSTR_REPEAT_ITERATION(BX_CPU_ID, i);
          RCX --;
        }
        if (! get_ZF() || RCX == 0) return;

        TEST_FOR_ANOTHER_ITERATION
      }
    }
    else
    if (i->as32L()) {
      while(1) {
        if (ECX != 0) {
          BX_CPU_CALL_METHOD(execute, (i));
          BX_INSTR_REPEAT_ITERATION(BX_CPU_ID, i);
          RCX = ECX - 1;
        }
        if (! get_ZF() || ECX == 0) return;

        TEST_FOR_ANOTHER_ITERATION
      }
    }
    else  // 16bit addrsize
    {
      BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
    }
  }
  else {          /* repeat prefix 0xF2 */
    if (i->as64L()) {
      while(1) {
        if (RCX != 0) {
          BX_CPU_CALL_METHOD(execute, (i));
          BX_INSTR_REPEAT_ITERATION(BX_CPU_ID, i);
          RCX --;
        }
        if (get_ZF() || RCX == 0) return;

        TEST_FOR_ANOTHER_ITERATION
      }
    }
    else
    if (i->as32L()) {
      while(1) {
        if (ECX != 0) {
          BX_CPU_CALL_METHOD(execute, (i));
          BX_INSTR_REPEAT_ITERATION(BX_CPU_ID, i);
          RCX = ECX - 1;
        }
        if (get_ZF() || ECX == 0) return;

        TEST_FOR_ANOTHER_ITERATION
      }
    }
    else  // 16bit addrsize
    {
      BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
    }
  }
}


  void BX_CPP_AttrRegparmN(3)
BX_CPU_C::write_virtual_byte_32(unsigned s, Bit32u offset, Bit8u data)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}

  void BX_CPP_AttrRegparmN(3)
BX_CPU_C::write_virtual_word_32(unsigned s, Bit32u offset, Bit16u data)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}

  void BX_CPP_AttrRegparmN(3)
BX_CPU_C::write_virtual_dword_32(unsigned s, Bit32u offset, Bit32u data)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}

  void BX_CPP_AttrRegparmN(3)
BX_CPU_C::write_virtual_qword_32(unsigned s, Bit32u offset, Bit64u data)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}

#if BX_CPU_LEVEL >= 6

  void BX_CPP_AttrRegparmN(3)
BX_CPU_C::write_virtual_dqword_32(unsigned s, Bit32u offset, const BxPackedXmmRegister *data)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}

  void BX_CPP_AttrRegparmN(3)
BX_CPU_C::write_virtual_dqword_aligned_32(unsigned s, Bit32u offset, const BxPackedXmmRegister *data)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}

#endif

  Bit8u BX_CPP_AttrRegparmN(2)
BX_CPU_C::read_virtual_byte_32(unsigned s, Bit32u offset)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
  return 0;
}

  Bit16u BX_CPP_AttrRegparmN(2)
BX_CPU_C::read_virtual_word_32(unsigned s, Bit32u offset)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
  return 0;
}

  Bit32u BX_CPP_AttrRegparmN(2)
BX_CPU_C::read_virtual_dword_32(unsigned s, Bit32u offset)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
  return 0;
}

  Bit64u BX_CPP_AttrRegparmN(2)
BX_CPU_C::read_virtual_qword_32(unsigned s, Bit32u offset)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
  return 0;
}


#if BX_CPU_LEVEL >= 6

  void BX_CPP_AttrRegparmN(3)
BX_CPU_C::read_virtual_dqword_32(unsigned s, Bit32u offset, BxPackedXmmRegister *data)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}

  void BX_CPP_AttrRegparmN(3)
BX_CPU_C::read_virtual_dqword_aligned_32(unsigned s, Bit32u offset, BxPackedXmmRegister *data)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
}

#endif

//////////////////////////////////////////////////////////////
// special Read-Modify-Write operations                     //
// address translation info is kept across read/write calls //
//////////////////////////////////////////////////////////////

  Bit8u BX_CPP_AttrRegparmN(2)
BX_CPU_C::read_RMW_virtual_byte_32(unsigned s, Bit32u offset)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
  return 0;
}

  Bit16u BX_CPP_AttrRegparmN(2)
BX_CPU_C::read_RMW_virtual_word_32(unsigned s, Bit32u offset)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
  return 0;
}

  Bit32u BX_CPP_AttrRegparmN(2)
BX_CPU_C::read_RMW_virtual_dword_32(unsigned s, Bit32u offset)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
  return 0;
}

  Bit64u BX_CPP_AttrRegparmN(2)
BX_CPU_C::read_RMW_virtual_qword_32(unsigned s, Bit32u offset)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
  return 0;
}

