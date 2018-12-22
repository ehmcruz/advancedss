/////////////////////////////////////////////////////////////////////////
// $Id: ctrl_xfer64.cc,v 1.80 2009/04/05 18:16:29 sshwarts Exp $
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
#include "../bochs_fake.h"
#include "../../cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

using namespace cpu;

#if BX_SUPPORT_X86_64

BX_CPP_INLINE void BX_CPP_AttrRegparmN(1) BX_CPU_C::branch_near64(bxInstruction_c *i)
{
  Bit64u new_RIP = RIP + (Bit32s) i->Id();

  if (! IsCanonical(new_RIP)) {
    BX_ERROR(("branch_near64: canonical RIP violation"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  RIP = new_RIP;
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::RETnear64_Iw(bxInstruction_c *i)
{
#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_ret;
#endif

  Bit64u return_RIP = read_virtual_qword_64(BX_SEG_REG_SS, RSP);

  if (! IsCanonical(return_RIP)) {
    BX_ERROR(("RETnear64_Iw: canonical RIP violation"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  RIP = return_RIP;
  RSP += 8 + i->Iw();

  BX_INSTR_UCNEAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_RET, RIP);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::RETnear64(bxInstruction_c *i)
{
#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_ret;
#endif

  Bit64u return_RIP = read_virtual_qword_64(BX_SEG_REG_SS, RSP);

  if (! IsCanonical(return_RIP)) {
    BX_ERROR(("RETnear64: canonical RIP violation %08x%08x", GET32H(return_RIP), GET32L(return_RIP)));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  RIP = return_RIP;
  RSP += 8;

  BX_INSTR_UCNEAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_RET, RIP);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::RETfar64_Iw(bxInstruction_c *i)
{
  BOCHS_MOD_INVALID_INSTRUCTION
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::CALL_Jq(bxInstruction_c *i)
{
  Bit64u new_RIP = RIP + (Bit32s) i->Id();

#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_call;
#endif

  /* push 64 bit EA of next instruction */
  write_virtual_qword_64(BX_SEG_REG_SS, RSP-8, RIP);

  if (! IsCanonical(new_RIP)) {
    BX_ERROR(("CALL_Jq: canonical RIP violation"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  RIP = new_RIP;
  RSP -= 8;

  BX_INSTR_UCNEAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_CALL, RIP);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::CALL_EqR(bxInstruction_c *i)
{
#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_call;
#endif

  Bit64u new_RIP = BX_READ_64BIT_REG(i->rm());

  /* push 64 bit EA of next instruction */
  write_virtual_qword_64(BX_SEG_REG_SS, RSP-8, RIP);

  if (! IsCanonical(new_RIP))
  {
    BX_ERROR(("CALL_Eq: canonical RIP violation"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  RIP = new_RIP;
  RSP -= 8;

  BX_INSTR_UCNEAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_CALL, RIP);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::CALL64_Ep(bxInstruction_c *i)
{
  BOCHS_MOD_INVALID_INSTRUCTION
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JMP_Jq(bxInstruction_c *i)
{
  Bit64u new_RIP = RIP + (Bit32s) i->Id();

  if (! IsCanonical(new_RIP)) {
    BX_ERROR(("JMP_Jq: canonical RIP violation"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  RIP = new_RIP;

  BX_INSTR_UCNEAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_JMP, RIP);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JO_Jq(bxInstruction_c *i)
{
  if (get_OF()) {
    branch_near64(i);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, RIP);
  }
#if BX_INSTRUMENTATION
  else {
    BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
  }
#endif
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JNO_Jq(bxInstruction_c *i)
{
  if (! get_OF()) {
    branch_near64(i);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, RIP);
  }
#if BX_INSTRUMENTATION
  else {
    BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
  }
#endif
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JB_Jq(bxInstruction_c *i)
{
  if (get_CF()) {
    branch_near64(i);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, RIP);
  }
#if BX_INSTRUMENTATION
  else {
    BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
  }
#endif
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JNB_Jq(bxInstruction_c *i)
{
  if (! get_CF()) {
    branch_near64(i);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, RIP);
  }
#if BX_INSTRUMENTATION
  else {
    BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
  }
#endif
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JZ_Jq(bxInstruction_c *i)
{
  if (get_ZF()) {
    branch_near64(i);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, RIP);
  }
#if BX_INSTRUMENTATION
  else {
    BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
  }
#endif
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JNZ_Jq(bxInstruction_c *i)
{
  if (! get_ZF()) {
    branch_near64(i);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, RIP);
  }
#if BX_INSTRUMENTATION
  else {
    BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
  }
#endif
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JBE_Jq(bxInstruction_c *i)
{
  if (get_CF() || get_ZF()) {
    branch_near64(i);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, RIP);
  }
#if BX_INSTRUMENTATION
  else {
    BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
  }
#endif
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JNBE_Jq(bxInstruction_c *i)
{
  if (! (get_CF() || get_ZF())) {
    branch_near64(i);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, RIP);
  }
#if BX_INSTRUMENTATION
  else {
    BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
  }
#endif
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JS_Jq(bxInstruction_c *i)
{
  if (get_SF()) {
    branch_near64(i);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, RIP);
  }
#if BX_INSTRUMENTATION
  else {
    BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
  }
#endif
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JNS_Jq(bxInstruction_c *i)
{
  if (! get_SF()) {
    branch_near64(i);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, RIP);
  }
#if BX_INSTRUMENTATION
  else {
    BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
  }
#endif
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JP_Jq(bxInstruction_c *i)
{
  if (get_PF()) {
    branch_near64(i);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, RIP);
  }
#if BX_INSTRUMENTATION
  else {
    BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
  }
#endif
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JNP_Jq(bxInstruction_c *i)
{
  if (! get_PF()) {
    branch_near64(i);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, RIP);
  }
#if BX_INSTRUMENTATION
  else {
    BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
  }
#endif
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JL_Jq(bxInstruction_c *i)
{
  if (getB_SF() != getB_OF()) {
    branch_near64(i);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, RIP);
  }
#if BX_INSTRUMENTATION
  else {
    BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
  }
#endif
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JNL_Jq(bxInstruction_c *i)
{
  if (getB_SF() == getB_OF()) {
    branch_near64(i);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, RIP);
  }
#if BX_INSTRUMENTATION
  else {
    BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
  }
#endif
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JLE_Jq(bxInstruction_c *i)
{
  if (get_ZF() || (getB_SF() != getB_OF())) {
    branch_near64(i);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, RIP);
  }
#if BX_INSTRUMENTATION
  else {
    BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
  }
#endif
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JNLE_Jq(bxInstruction_c *i)
{
  if (! get_ZF() && (getB_SF() == getB_OF())) {
    branch_near64(i);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, RIP);
  }
#if BX_INSTRUMENTATION
  else {
    BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
  }
#endif
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JMP_EqR(bxInstruction_c *i)
{
  Bit64u op1_64 = BX_READ_64BIT_REG(i->rm());

  if (! IsCanonical(op1_64)) {
    BX_ERROR(("JMP_Eq: canonical RIP violation"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  RIP = op1_64;

  BX_INSTR_UCNEAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_JMP, RIP);
}

/* Far indirect jump */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::JMP64_Ep(bxInstruction_c *i)
{
  BOCHS_MOD_INVALID_INSTRUCTION
}

// here, we set iret to behave as advancedss expects

void BX_CPP_AttrRegparmN(1) BX_CPU_C::IRET64(bxInstruction_c *i)
{
  //RSP_SPECULATIVE;

	uint64_t addr, rflags, rsp, rip;

	CPU_X8664_CHECK_KERNEL_MODE

	addr = CPU_X8664_READ_MSR(CPU_X8664_MSR_KERNEL_STACK_PTR);

	addr -= 8;
	rflags = read_virtual_qword_64(0, addr);

	addr -= 8;
	rsp = read_virtual_qword_64(0, addr);

	addr -= 8;
	rip = read_virtual_qword_64(0, addr);

	RSP = rsp;
	RIP = rip;
	this->setEFlags((uint32_t)rflags);

	CPU_X8664_SET_CPL(CPU_X8664_CPL_USER)
	BOCHS_CPU_DECODER_INST_MUST_FLUSH_PIPELINE

  //RSP_COMMIT;
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::JRCXZ_Jb(bxInstruction_c *i)
{
  Bit64u temp_RCX;

  if (i->as64L())
    temp_RCX = RCX;
  else
    temp_RCX = ECX;

  if (temp_RCX == 0) {
    branch_near64(i);
    BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, RIP);
  }
#if BX_INSTRUMENTATION
  else {
    BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
  }
#endif
}

//
// There is some weirdness in LOOP instructions definition. If an exception
// was generated during the instruction execution (for example #GP fault
// because EIP was beyond CS segment limits) CPU state should restore the
// state prior to instruction execution.
//
// The final point that we are not allowed to decrement RCX register before
// it is known that no exceptions can happen.
//

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOOPNE64_Jb(bxInstruction_c *i)
{
  if (i->as64L()) {
    Bit64u count = RCX;

    if (((--count) != 0) && (get_ZF()==0)) {
      branch_near64(i);
      BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, RIP);
    }
#if BX_INSTRUMENTATION
    else {
      BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
    }
#endif

    RCX = count;
  }
  else {
    Bit32u count = ECX;

    if (((--count) != 0) && (get_ZF()==0)) {
      branch_near64(i);
      BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, RIP);
    }
#if BX_INSTRUMENTATION
    else {
      BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
    }
#endif

    RCX = count;
  }
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOOPE64_Jb(bxInstruction_c *i)
{
  if (i->as64L()) {
    Bit64u count = RCX;

    if (((--count) != 0) && get_ZF()) {
      branch_near64(i);
      BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, RIP);
    }
#if BX_INSTRUMENTATION
    else {
      BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
    }
#endif

    RCX = count;
  }
  else {
    Bit32u count = ECX;

    if (((--count) != 0) && get_ZF()) {
      branch_near64(i);
      BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, RIP);
    }
#if BX_INSTRUMENTATION
    else {
      BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
    }
#endif

    RCX = count;
  }
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LOOP64_Jb(bxInstruction_c *i)
{
  if (i->as64L()) {
    Bit64u count = RCX;

    if ((--count) != 0) {
      branch_near64(i);
      BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, RIP);
    }
#if BX_INSTRUMENTATION
    else {
      BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
    }
#endif

    RCX = count;
  }
  else {
    Bit32u count = ECX;

    if ((--count) != 0) {
      branch_near64(i);
      BX_INSTR_CNEAR_BRANCH_TAKEN(BX_CPU_ID, RIP);
    }
#if BX_INSTRUMENTATION
    else {
      BX_INSTR_CNEAR_BRANCH_NOT_TAKEN(BX_CPU_ID);
    }
#endif

    RCX = count;
  }
}

#endif /* if BX_SUPPORT_X86_64 */
