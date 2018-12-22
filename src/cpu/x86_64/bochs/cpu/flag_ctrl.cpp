/////////////////////////////////////////////////////////////////////////
// $Id: flag_ctrl.cc,v 1.45 2009/03/27 16:42:21 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002  MandrakeSoft S.A.
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

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SAHF(bxInstruction_c *i)
{
  set_SF((AH & 0x80) >> 7);
  set_ZF((AH & 0x40) >> 6);
  set_AF((AH & 0x10) >> 4);
  set_CF (AH & 0x01);
  set_PF((AH & 0x04) >> 2);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LAHF(bxInstruction_c *i)
{
  AH = read_eflags() & 0xFF;
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::CLC(bxInstruction_c *i)
{
  clear_CF();
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::STC(bxInstruction_c *i)
{
  assert_CF();
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::CLI(bxInstruction_c *i)
{
	CPU_X8664_CHECK_KERNEL_MODE
	CPU_X86_64_DISABLE_INTERRUPTIONS
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::STI(bxInstruction_c *i)
{
	CPU_X8664_CHECK_KERNEL_MODE
	CPU_X86_64_ENABLE_INTERRUPTIONS
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::CLD(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR clear_DF();
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::STD(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR assert_DF();
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::CMC(bxInstruction_c *i)
{
  set_CF(! get_CF());
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PUSHF_Fw(bxInstruction_c *i)
{
  Bit16u flags = (Bit16u) read_eflags();

  push_16(flags);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::POPF_Fw(bxInstruction_c *i)
{
  // Build a mask of the following bits:
  // x,NT,IOPL,OF,DF,IF,TF,SF,ZF,x,AF,x,PF,x,CF
  Bit32u changeMask = EFlagsOSZAPCMask | EFlagsTFMask | EFlagsDFMask;
#if BX_CPU_LEVEL >= 3
  changeMask |= EFlagsNTMask;     // NT could be modified
#endif

  RSP_SPECULATIVE;

  Bit16u flags16 = pop_16();

  writeEFlags((Bit32u) flags16, changeMask);

  RSP_COMMIT;
}

#if BX_CPU_LEVEL >= 3

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PUSHF_Fd(bxInstruction_c *i)
{
  // VM & RF flags cleared in image stored on the stack
  push_32(read_eflags() & 0x00fcffff);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::POPF_Fd(bxInstruction_c *i)
{
  // Build a mask of the following bits:
  // ID,VIP,VIF,AC,VM,RF,x,NT,IOPL,OF,DF,IF,TF,SF,ZF,x,AF,x,PF,x,CF
  Bit32u changeMask = EFlagsOSZAPCMask | EFlagsTFMask |
                          EFlagsDFMask | EFlagsNTMask | EFlagsRFMask;
#if BX_CPU_LEVEL >= 4
  changeMask |= (EFlagsIDMask | EFlagsACMask);  // ID/AC
#endif

  RSP_SPECULATIVE;

  Bit32u flags32 = pop_32();

  writeEFlags(flags32, changeMask);

  RSP_COMMIT;
}

#if BX_SUPPORT_X86_64
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PUSHF_Fq(bxInstruction_c *i)
{
  // VM & RF flags cleared in image stored on the stack
  push_64(read_eflags() & 0x00fcffff);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::POPF_Fq(bxInstruction_c *i)
{
  // Build a mask of the following bits:
  // ID,VIP,VIF,AC,VM,RF,x,NT,IOPL,OF,DF,IF,TF,SF,ZF,x,AF,x,PF,x,CF
  Bit32u changeMask = EFlagsOSZAPCMask | EFlagsTFMask | EFlagsDFMask
                        | EFlagsNTMask | EFlagsRFMask | EFlagsACMask
                        | EFlagsIDMask;

  Bit32u eflags32 = (Bit32u) pop_64();

  // VIF, VIP, VM are unaffected
  writeEFlags(eflags32, changeMask);
}
#endif

#endif  // BX_CPU_LEVEL >= 3

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SALC(bxInstruction_c *i)
{
  if (get_CF()) {
    AL = 0xff;
  }
  else {
    AL = 0x00;
  }
}
