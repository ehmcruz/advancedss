/////////////////////////////////////////////////////////////////////////
// $Id: proc_ctrl.cc,v 1.295.2.1 2009/06/07 07:49:10 vruppert Exp $
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
#include "../bochs_fake.h"
#include "../../cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

using namespace cpu;

#if BX_SUPPORT_X86_64==0
// Make life easier for merging code.
#define RAX EAX
#define RCX ECX
#define RDX EDX
#define RIP EIP
#endif

void BX_CPP_AttrRegparmN(1) BX_CPU_C::UndefinedOpcode(bxInstruction_c *i)
{
  BX_DEBUG(("UndefinedOpcode: b1 = 0x%02x causes #UD exception", i->b1()));
  exception(BX_UD_EXCEPTION, 0, 0);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::NOP(bxInstruction_c *i)
{
  // No operation.
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PAUSE(bxInstruction_c *i)
{
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PREFETCH(bxInstruction_c *i)
{

}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::HLT(bxInstruction_c *i)
{
	CPU_X8664_CHECK_KERNEL_MODE
	BOCHS_SET_RESULT_TYPE_SYSTEM_HALT
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::CLTS(bxInstruction_c *i)
{
  BOCHS_MOD_INVALID_INSTRUCTION
}

/* 0F 08 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::INVD(bxInstruction_c *i)
{
  BOCHS_MOD_INVALID_INSTRUCTION
}

/* 0F 09 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::WBINVD(bxInstruction_c *i)
{
  BOCHS_MOD_INVALID_INSTRUCTION
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::CLFLUSH(bxInstruction_c *i)
{
  BOCHS_MOD_INVALID_INSTRUCTION
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_DdRd(bxInstruction_c *i)
{
  BOCHS_MOD_INVALID_INSTRUCTION
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_RdDd(bxInstruction_c *i)
{
  BOCHS_MOD_INVALID_INSTRUCTION
}

#if BX_SUPPORT_X86_64
void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_DqRq(bxInstruction_c *i)
{
  BOCHS_MOD_INVALID_INSTRUCTION
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_RqDq(bxInstruction_c *i)
{
  BOCHS_MOD_INVALID_INSTRUCTION
}
#endif // #if BX_SUPPORT_X86_64

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_CdRd(bxInstruction_c *i)
{
  BOCHS_MOD_INVALID_INSTRUCTION
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_RdCd(bxInstruction_c *i)
{
  BOCHS_MOD_INVALID_INSTRUCTION
}

#if BX_SUPPORT_X86_64
void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_CqRq(bxInstruction_c *i)
{
  BOCHS_MOD_INVALID_INSTRUCTION
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_RqCq(bxInstruction_c *i)
{
  BOCHS_MOD_INVALID_INSTRUCTION
}
#endif // #if BX_SUPPORT_X86_64

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LMSW_Ew(bxInstruction_c *i)
{
  BOCHS_MOD_INVALID_INSTRUCTION
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SMSW_EwR(bxInstruction_c *i)
{
  BOCHS_MOD_INVALID_INSTRUCTION
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SMSW_EwM(bxInstruction_c *i)
{
  BOCHS_MOD_INVALID_INSTRUCTION
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_TdRd(bxInstruction_c *i)
{
  BOCHS_MOD_INVALID_INSTRUCTION
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_RdTd(bxInstruction_c *i)
{
  BOCHS_MOD_INVALID_INSTRUCTION
}

#if BX_CPU_LEVEL == 2
void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOADALL(bxInstruction_c *i)
{
  BOCHS_MOD_INVALID_INSTRUCTION
}
#endif

#if BX_CPU_LEVEL >= 4 && BX_SUPPORT_ALIGNMENT_CHECK
void BX_CPU_C::handleAlignmentCheck(void)
{
  if (CPL == 3 && BX_CPU_THIS_PTR cr0.get_AM() && BX_CPU_THIS_PTR get_AC()) {
    if (BX_CPU_THIS_PTR alignment_check_mask == 0) {
      BX_CPU_THIS_PTR alignment_check_mask = 0xF;
      BX_INFO(("Enable alignment check (#AC exception)"));
      BX_CPU_THIS_PTR iCache.flushICacheEntries();
    }
  }
  else {
    if (BX_CPU_THIS_PTR alignment_check_mask != 0) {
      BX_CPU_THIS_PTR alignment_check_mask = 0;
      BX_INFO(("Disable alignment check (#AC exception)"));
      BX_CPU_THIS_PTR iCache.flushICacheEntries();
    }
  }
}
#endif

void BX_CPP_AttrRegparmN(1) BX_CPU_C::RDPMC(bxInstruction_c *i)
{
  BOCHS_MOD_INVALID_INSTRUCTION
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::RDTSC(bxInstruction_c *i)
{
  BOCHS_MOD_INVALID_INSTRUCTION
}

#if BX_SUPPORT_X86_64
void BX_CPP_AttrRegparmN(1) BX_CPU_C::RDTSCP(bxInstruction_c *i)
{
  BOCHS_MOD_INVALID_INSTRUCTION
}
#endif

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MONITOR(bxInstruction_c *i)
{
  BOCHS_MOD_INVALID_INSTRUCTION
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MWAIT(bxInstruction_c *i)
{
  BOCHS_MOD_INVALID_INSTRUCTION
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SYSENTER(bxInstruction_c *i)
{
  BOCHS_MOD_INVALID_INSTRUCTION
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SYSEXIT(bxInstruction_c *i)
{
  BOCHS_MOD_INVALID_INSTRUCTION
}

#if BX_SUPPORT_X86_64
void BX_CPP_AttrRegparmN(1) BX_CPU_C::SYSCALL(bxInstruction_c *i)
{
  BOCHS_SET_RESULT_TYPE_TRAP(0xFFFF)
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SYSRET(bxInstruction_c *i)
{
  BOCHS_MOD_INVALID_INSTRUCTION
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SWAPGS(bxInstruction_c *i)
{
  BOCHS_MOD_INVALID_INSTRUCTION
}
#endif
