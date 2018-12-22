/////////////////////////////////////////////////////////////////////////
// $Id: flag_ctrl_pro.cc,v 1.37 2009/02/13 20:09:56 sshwarts Exp $
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

void BX_CPP_AttrRegparmN(1) BX_CPU_C::setEFlags(Bit32u val)
{
  BX_CPU_THIS_PTR eflags = val;
  BX_CPU_THIS_PTR lf_flags_status = 0; // OSZAPC flags are known.
}

  void BX_CPP_AttrRegparmN(2)
BX_CPU_C::writeEFlags(Bit32u flags, Bit32u changeMask)
{
  // Build a mask of the non-reserved bits:
  // ID,VIP,VIF,AC,VM,RF,x,NT,IOPL,OF,DF,IF,TF,SF,ZF,x,AF,x,PF,x,CF
  Bit32u supportMask = 0x00037fd5;
#if BX_CPU_LEVEL >= 4
  supportMask |= (EFlagsIDMask | EFlagsACMask); // ID/AC
#endif
#if BX_SUPPORT_VME
  supportMask |= (EFlagsVIPMask | EFlagsVIFMask); // VIP/VIF
#endif

  // Screen out changing of any unsupported bits.
  changeMask &= supportMask;

  Bit32u newEFlags = (BX_CPU_THIS_PTR eflags & ~changeMask) |
              (flags & changeMask);
  setEFlags(newEFlags);
  // OSZAPC flags are known - done in setEFlags(newEFlags)
}

// Cause arithmetic flags to be in known state and cached in val32.
Bit32u BX_CPU_C::force_flags(void)
{
  if (BX_CPU_THIS_PTR lf_flags_status) {
    Bit32u newflags;

    newflags  = get_CF() ? EFlagsCFMask : 0;
    newflags |= get_PF() ? EFlagsPFMask : 0;
    newflags |= get_AF() ? EFlagsAFMask : 0;
    newflags |= get_ZF() ? EFlagsZFMask : 0;
    newflags |= get_SF() ? EFlagsSFMask : 0;
    newflags |= get_OF() ? EFlagsOFMask : 0;

    setEFlagsOSZAPC(newflags);
  }

  return BX_CPU_THIS_PTR eflags;
}
