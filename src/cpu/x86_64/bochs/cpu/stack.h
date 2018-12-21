/////////////////////////////////////////////////////////////////////////
// $Id: stack.h,v 1.4 2009/01/16 18:18:59 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2007 Stanislav Shwartsman
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

#ifndef BX_PUSHPOP_H
#define BX_PUSHPOP_H

  BX_CPP_INLINE void BX_CPP_AttrRegparmN(1)
BX_CPU_C::push_16(Bit16u value16)
{
    write_virtual_word_64(BX_SEG_REG_SS, RSP-2, value16);
    RSP -= 2;
}

  BX_CPP_INLINE void BX_CPP_AttrRegparmN(1)
BX_CPU_C::push_32(Bit32u value32)
{
    write_virtual_dword_64(BX_SEG_REG_SS, RSP-4, value32);
    RSP -= 4;
}

/* push 64 bit operand */
  BX_CPP_INLINE void BX_CPP_AttrRegparmN(1)
BX_CPU_C::push_64(Bit64u value64)
{
  write_virtual_qword_64(BX_SEG_REG_SS, RSP-8, value64);
  RSP -= 8;
}


/* pop 16 bit operand from the stack */
BX_CPP_INLINE Bit16u BX_CPU_C::pop_16(void)
{
  Bit16u value16;

    value16 = read_virtual_word_64(BX_SEG_REG_SS, RSP);
    RSP += 2;

  return value16;
}

/* pop 32 bit operand from the stack */
BX_CPP_INLINE Bit32u BX_CPU_C::pop_32(void)
{
  Bit32u value32;

    value32 = read_virtual_dword_64(BX_SEG_REG_SS, RSP);
    RSP += 4;

  return value32;
}

/* pop 64 bit operand from the stack */
BX_CPP_INLINE Bit64u BX_CPU_C::pop_64(void)
{
  Bit64u value64 = read_virtual_qword_64(BX_SEG_REG_SS, RSP);
  RSP += 8;

  return value64;
}

#endif
