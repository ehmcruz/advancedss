/////////////////////////////////////////////////////////////////////////
// $Id: resolver.cc,v 1.3 2009/01/16 18:18:58 sshwarts Exp $
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
#include "../../cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

using namespace cpu;

//
// 32 bit address size
//

  bx_address  BX_CPP_AttrRegparmN(1)
BX_CPU_C::BxResolve32Base(bxInstruction_c *i)
{
  return (Bit32u) (BX_READ_32BIT_REG(i->sibBase()) + i->displ32s());
}
  bx_address  BX_CPP_AttrRegparmN(1)
BX_CPU_C::BxResolve32BaseIndex(bxInstruction_c *i)
{
  return (Bit32u) (BX_READ_32BIT_REG(i->sibBase()) + (BX_READ_32BIT_REG(i->sibIndex()) << i->sibScale()) + i->displ32s());
}

//
// 64 bit address size
//


  bx_address  BX_CPP_AttrRegparmN(1)
BX_CPU_C::BxResolve64Base(bxInstruction_c *i)
{
  return BX_READ_64BIT_REG(i->sibBase()) + i->displ32s();
}
  bx_address  BX_CPP_AttrRegparmN(1)
BX_CPU_C::BxResolve64BaseIndex(bxInstruction_c *i)
{
  return BX_READ_64BIT_REG(i->sibBase()) + (BX_READ_64BIT_REG(i->sibIndex()) << i->sibScale()) + i->displ32s();
}

