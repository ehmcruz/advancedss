/////////////////////////////////////////////////////////////////////////
// $Id:  1.78 2009/04/07 16:12:19 sshwarts Exp $
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

#ifndef _ADVANCEDSS_X8664_BOCHS_GENERAL_END_HEADER_
#define _ADVANCEDSS_X8664_BOCHS_GENERAL_END_HEADER_

#include "bochs.h"

#if BX_SUPPORT_SSE
BX_CPP_INLINE void BX_CPU_C::prepareSSE(void)
{
/*  if(BX_CPU_THIS_PTR cr0.get_EM() || !BX_CPU_THIS_PTR cr4.get_OSFXSR())
    exception(BX_UD_EXCEPTION, 0, 0);

  if(BX_CPU_THIS_PTR cr0.get_TS())
    exception(BX_NM_EXCEPTION, 0, 0);*/
}
#endif

#if BX_SUPPORT_MMX
BX_CPP_INLINE void BX_CPU_C::prepareMMX(void)
{
/*  if(BX_CPU_THIS_PTR cr0.get_EM())
    exception(BX_UD_EXCEPTION, 0, 0);

  if(BX_CPU_THIS_PTR cr0.get_TS())
    exception(BX_NM_EXCEPTION, 0, 0);*/

  /* check floating point status word for a pending FPU exceptions */
  FPU_check_pending_exceptions();
}
#endif

// Can be used as LHS or RHS.
#define RMAddr(i)  (BX_CPU_THIS_PTR address_xlation.rm_addr)

#define write_virtual_byte_64(seg, offset, data)  \
      write_virtual_byte(seg, offset, data)

#define write_virtual_word_64(seg, offset, data)  \
      write_virtual_word(seg, offset, data)

#define write_virtual_dword_64(seg, offset, data)  \
      write_virtual_dword(seg, offset, data)

#define write_virtual_qword_64(seg, offset, data)  \
      write_virtual_qword(seg, offset, data)

///////////////

#define read_virtual_byte_64(seg, offset)          \
      read_virtual_byte(seg, offset)

#define read_virtual_word_64(seg, offset)          \
      read_virtual_word(seg, offset)

#define read_virtual_dword_64(seg, offset)          \
      read_virtual_dword(seg, offset)

#define read_virtual_qword_64(seg, offset)          \
      read_virtual_qword(seg, offset)

// write
#define write_virtual_byte(seg, offset, data)     \
      this->bochs_mem_write_1_bytes((Bit64u) offset, data)

#define write_virtual_word(seg, offset, data)     \
      this->bochs_mem_write_2_bytes((Bit64u) offset, data)

#define write_virtual_dword(seg, offset, data)    \
      this->bochs_mem_write_4_bytes((Bit64u) offset, data)

#define write_virtual_qword(seg, offset, data)    \
      this->bochs_mem_write_8_bytes((Bit64u) offset, data)

#define write_virtual_dqword(seg, offset, data)   \
      this->bochs_mem_write_PackedXmmRegister((Bit64u) offset, (BxPackedXmmRegister*)(data))

#define write_virtual_dqword_aligned(seg, offset, data) \
      this->bochs_mem_write_PackedXmmRegister_aligned((Bit64u) offset, (BxPackedXmmRegister*)(data))

// read
#define read_virtual_byte(seg, offset)             \
      this->bochs_mem_read_1_bytes((Bit64u) offset)

#define read_virtual_word(seg, offset)             \
      this->bochs_mem_read_2_bytes((Bit64u) offset)

#define read_virtual_dword(seg, offset)             \
      this->bochs_mem_read_4_bytes((Bit64u) offset)

#define read_virtual_qword(seg, offset)             \
      this->bochs_mem_read_8_bytes((Bit64u) offset)

#define read_virtual_dqword(seg, offset, data)    \
      this->bochs_mem_read_PackedXmmRegister((Bit64u) offset, (BxPackedXmmRegister*)(data))

#define read_virtual_dqword_aligned(seg, offset, data) \
      this->bochs_mem_read_PackedXmmRegister_aligned((Bit64u) offset, (BxPackedXmmRegister*)(data))

#define readVirtualDQwordAligned(s, off, data) \
      read_virtual_dqword_aligned(s, off, data);

// RMW
#define read_RMW_virtual_byte(seg, offset)        \
      this->bochs_mem_RMW_read_1_bytes((Bit64u) offset)

#define read_RMW_virtual_word(seg, offset)        \
      this->bochs_mem_RMW_read_2_bytes((Bit64u) offset)

#define read_RMW_virtual_dword(seg, offset)       \
      this->bochs_mem_RMW_read_4_bytes((Bit64u) offset)

#define read_RMW_virtual_qword(seg, offset)       \
      this->bochs_mem_RMW_read_8_bytes((Bit64u) offset)

/////////////////

#define write_RMW_virtual_byte(data)              \
      this->bochs_mem_RMW_write_1_bytes(data)

#define write_RMW_virtual_word(data)              \
      this->bochs_mem_RMW_write_2_bytes(data)

#define write_RMW_virtual_dword(data)              \
      this->bochs_mem_RMW_write_4_bytes(data)

#define write_RMW_virtual_qword(data)              \
      this->bochs_mem_RMW_write_8_bytes(data)

//////////////////

#define read_RMW_virtual_byte_64(seg, offset)      \
      read_RMW_virtual_byte(seg, offset)

#define read_RMW_virtual_word_64(seg, offset)      \
      read_RMW_virtual_word(seg, offset)

#define read_RMW_virtual_dword_64(seg, offset)      \
      read_RMW_virtual_dword(seg, offset)

#define read_RMW_virtual_qword_64(seg, offset)      \
      read_RMW_virtual_qword(seg, offset)

extern const Bit8u bx_parity_lookup[256];

BX_CPP_INLINE void BX_CPU_C::set_PF_base(Bit8u val)
{
  BX_CPU_THIS_PTR lf_flags_status &= ~EFlagsPFMask;
  val = bx_parity_lookup[val]; // Always returns 0 or 1.
  BX_CPU_THIS_PTR eflags &= ~(1<<2);
  BX_CPU_THIS_PTR eflags |= val<<2;
}

//
// inline simple lazy flags implementation methods
//
BX_CPP_INLINE bx_bool BX_CPU_C::get_ZFLazy(void)
{
  return (BX_CPU_THIS_PTR oszapc.result == 0);
}

BX_CPP_INLINE bx_bool BX_CPU_C::get_SFLazy(void)
{
  return (BX_CPU_THIS_PTR oszapc.result >> BX_LF_SIGN_BIT);
}

BX_CPP_INLINE bx_bool BX_CPU_C::get_PFLazy(void)
{
  return bx_parity_lookup[(Bit8u) BX_CPU_THIS_PTR oszapc.result];
}

// *******************
// OSZAPC
// *******************

/* op1, op2, result */
#define SET_FLAGS_OSZAPC_SIZE(size, lf_op1, lf_op2, lf_result, ins) { \
  BX_CPU_THIS_PTR oszapc.op1    = (bx_address)(Bit##size##s)(lf_op1); \
  BX_CPU_THIS_PTR oszapc.op2    = (bx_address)(Bit##size##s)(lf_op2); \
  BX_CPU_THIS_PTR oszapc.result = (bx_address)(Bit##size##s)(lf_result); \
  BX_CPU_THIS_PTR oszapc.instr = (ins); \
  BX_CPU_THIS_PTR lf_flags_status = EFlagsOSZAPCMask; \
}

#define SET_FLAGS_OSZAPC_8(op1, op2, result, ins) \
  SET_FLAGS_OSZAPC_SIZE(8, op1, op2, result, ins)
#define SET_FLAGS_OSZAPC_16(op1, op2, result, ins) \
  SET_FLAGS_OSZAPC_SIZE(16, op1, op2, result, ins)
#define SET_FLAGS_OSZAPC_32(op1, op2, result, ins) \
  SET_FLAGS_OSZAPC_SIZE(32, op1, op2, result, ins)
#if BX_SUPPORT_X86_64
#define SET_FLAGS_OSZAPC_64(op1, op2, result, ins) \
  SET_FLAGS_OSZAPC_SIZE(64, op1, op2, result, ins)
#endif

/* op1 and result only */
#define SET_FLAGS_OSZAPC_S1_SIZE(size, lf_op1, lf_result, ins) { \
  BX_CPU_THIS_PTR oszapc.op1    = (bx_address)(Bit##size##s)(lf_op1); \
  BX_CPU_THIS_PTR oszapc.result = (Bit##size##s)(lf_result); \
  BX_CPU_THIS_PTR oszapc.instr = (ins); \
  BX_CPU_THIS_PTR lf_flags_status = EFlagsOSZAPCMask; \
}

#define SET_FLAGS_OSZAPC_S1_8(op1, result, ins) \
  SET_FLAGS_OSZAPC_S1_SIZE(8, op1, result, ins)
#define SET_FLAGS_OSZAPC_S1_16(op1, result, ins) \
  SET_FLAGS_OSZAPC_S1_SIZE(16, op1, result, ins)
#define SET_FLAGS_OSZAPC_S1_32(op1, result, ins) \
  SET_FLAGS_OSZAPC_S1_SIZE(32, op1, result, ins)
#if BX_SUPPORT_X86_64
#define SET_FLAGS_OSZAPC_S1_64(op1, result, ins) \
  SET_FLAGS_OSZAPC_S1_SIZE(64, op1, result, ins)
#endif

/* op2 and result only */
#define SET_FLAGS_OSZAPC_S2_SIZE(size, lf_op2, lf_result, ins) { \
  BX_CPU_THIS_PTR oszapc.op2    = (bx_address)(Bit##size##s)(lf_op2); \
  BX_CPU_THIS_PTR oszapc.result = (Bit##size##s)(lf_result); \
  BX_CPU_THIS_PTR oszapc.instr = (ins); \
  BX_CPU_THIS_PTR lf_flags_status = EFlagsOSZAPCMask; \
}

#define SET_FLAGS_OSZAPC_S2_8(op2, result, ins) \
  SET_FLAGS_OSZAPC_S2_SIZE(8, op2, result, ins)
#define SET_FLAGS_OSZAPC_S2_16(op2, result, ins) \
  SET_FLAGS_OSZAPC_S2_SIZE(16, op2, result, ins)
#define SET_FLAGS_OSZAPC_S2_32(op2, result, ins) \
  SET_FLAGS_OSZAPC_S2_SIZE(32, op2, result, ins)
#if BX_SUPPORT_X86_64
#define SET_FLAGS_OSZAPC_S2_64(op2, result, ins) \
  SET_FLAGS_OSZAPC_S2_SIZE(64, op2, result, ins)
#endif

/* result only */
#define SET_FLAGS_OSZAPC_RESULT_SIZE(size, lf_result, ins) { \
  BX_CPU_THIS_PTR oszapc.result = (Bit##size##s)(lf_result); \
  BX_CPU_THIS_PTR oszapc.instr = (ins); \
  BX_CPU_THIS_PTR lf_flags_status = EFlagsOSZAPCMask; \
}

#define SET_FLAGS_OSZAPC_RESULT_8(result, ins) \
  SET_FLAGS_OSZAPC_RESULT_SIZE(8, result, ins)
#define SET_FLAGS_OSZAPC_RESULT_16(result, ins) \
  SET_FLAGS_OSZAPC_RESULT_SIZE(16, result, ins)
#define SET_FLAGS_OSZAPC_RESULT_32(result, ins) \
  SET_FLAGS_OSZAPC_RESULT_SIZE(32, result, ins)
#if BX_SUPPORT_X86_64
#define SET_FLAGS_OSZAPC_RESULT_64(result, ins) \
  SET_FLAGS_OSZAPC_RESULT_SIZE(64, result, ins)
#endif

// *******************
// OSZAP
// *******************

/* result only */
#define SET_FLAGS_OSZAP_RESULT_SIZE(size, lf_result, ins) { \
  force_CF(); \
  BX_CPU_THIS_PTR oszapc.result = (Bit##size##s)(lf_result); \
  BX_CPU_THIS_PTR oszapc.instr = (ins); \
  BX_CPU_THIS_PTR lf_flags_status = EFlagsOSZAPMask; \
}

#define SET_FLAGS_OSZAP_RESULT_8(result, ins) \
  SET_FLAGS_OSZAP_RESULT_SIZE(8, result, ins)
#define SET_FLAGS_OSZAP_RESULT_16(result, ins) \
  SET_FLAGS_OSZAP_RESULT_SIZE(16, result, ins)
#define SET_FLAGS_OSZAP_RESULT_32(result, ins) \
  SET_FLAGS_OSZAP_RESULT_SIZE(32, result, ins)
#if BX_SUPPORT_X86_64
#define SET_FLAGS_OSZAP_RESULT_64(result, ins) \
  SET_FLAGS_OSZAP_RESULT_SIZE(64, result, ins)
#endif

// transition to new lazy flags code
#define SET_FLAGS_OSZAPC_LOGIC_8(result_8) \
   SET_FLAGS_OSZAPC_RESULT_8((result_8), BX_LF_INSTR_LOGIC8)
#define SET_FLAGS_OSZAPC_LOGIC_16(result_16) \
   SET_FLAGS_OSZAPC_RESULT_16((result_16), BX_LF_INSTR_LOGIC16)
#define SET_FLAGS_OSZAPC_LOGIC_32(result_32) \
   SET_FLAGS_OSZAPC_RESULT_32((result_32), BX_LF_INSTR_LOGIC32)
#if BX_SUPPORT_X86_64
#define SET_FLAGS_OSZAPC_LOGIC_64(result_64) \
   SET_FLAGS_OSZAPC_RESULT_64((result_64), BX_LF_INSTR_LOGIC64)
#endif

#define SET_FLAGS_OSZAPC_ADD_8(op1_8, op2_8, sum_8) \
  SET_FLAGS_OSZAPC_8((op1_8), (op2_8), (sum_8), BX_LF_INSTR_ADD8)
#define SET_FLAGS_OSZAPC_ADD_16(op1_16, op2_16, sum_16) \
  SET_FLAGS_OSZAPC_16((op1_16), (op2_16), (sum_16), BX_LF_INSTR_ADD16)
#define SET_FLAGS_OSZAPC_ADD_32(op1_32, op2_32, sum_32) \
  SET_FLAGS_OSZAPC_32((op1_32), (op2_32), (sum_32), BX_LF_INSTR_ADD32)
#if BX_SUPPORT_X86_64
#define SET_FLAGS_OSZAPC_ADD_64(op1_64, op2_64, sum_64) \
  SET_FLAGS_OSZAPC_64((op1_64), (op2_64), (sum_64), BX_LF_INSTR_ADD64)
#endif

#define SET_FLAGS_OSZAPC_SUB_8(op1_8, op2_8, diff_8) \
  SET_FLAGS_OSZAPC_8((op1_8), (op2_8), (diff_8), BX_LF_INSTR_SUB8)
#define SET_FLAGS_OSZAPC_SUB_16(op1_16, op2_16, diff_16) \
  SET_FLAGS_OSZAPC_16((op1_16), (op2_16), (diff_16), BX_LF_INSTR_SUB16)
#define SET_FLAGS_OSZAPC_SUB_32(op1_32, op2_32, diff_32) \
  SET_FLAGS_OSZAPC_32((op1_32), (op2_32), (diff_32), BX_LF_INSTR_SUB32)
#if BX_SUPPORT_X86_64
#define SET_FLAGS_OSZAPC_SUB_64(op1_64, op2_64, diff_64) \
  SET_FLAGS_OSZAPC_64((op1_64), (op2_64), (diff_64), BX_LF_INSTR_SUB64)
#endif

#define SET_FLAGS_OSZAPC_INC_8(result) \
  SET_FLAGS_OSZAP_RESULT_SIZE(8, (result), BX_LF_INSTR_INC8)
#define SET_FLAGS_OSZAPC_INC_16(result) \
  SET_FLAGS_OSZAP_RESULT_SIZE(16, (result), BX_LF_INSTR_INC16)
#define SET_FLAGS_OSZAPC_INC_32(result) \
  SET_FLAGS_OSZAP_RESULT_SIZE(32, (result), BX_LF_INSTR_INC32)
#if BX_SUPPORT_X86_64
#define SET_FLAGS_OSZAPC_INC_64(result) \
  SET_FLAGS_OSZAP_RESULT_SIZE(64, (result), BX_LF_INSTR_INC64)
#endif

#define SET_FLAGS_OSZAPC_DEC_8(result) \
  SET_FLAGS_OSZAP_RESULT_SIZE(8, (result), BX_LF_INSTR_DEC8)
#define SET_FLAGS_OSZAPC_DEC_16(result) \
  SET_FLAGS_OSZAP_RESULT_SIZE(16, (result), BX_LF_INSTR_DEC16)
#define SET_FLAGS_OSZAPC_DEC_32(result) \
  SET_FLAGS_OSZAP_RESULT_SIZE(32, (result), BX_LF_INSTR_DEC32)
#if BX_SUPPORT_X86_64
#define SET_FLAGS_OSZAPC_DEC_64(result) \
  SET_FLAGS_OSZAP_RESULT_SIZE(64, (result), BX_LF_INSTR_DEC64)
#endif

/*IMPLEMENT_EFLAG_ACCESSOR   (ID,  21)
IMPLEMENT_EFLAG_ACCESSOR   (VIP, 20)
IMPLEMENT_EFLAG_ACCESSOR   (VIF, 19)
IMPLEMENT_EFLAG_ACCESSOR   (AC,  18)
IMPLEMENT_EFLAG_ACCESSOR   (VM,  17)
IMPLEMENT_EFLAG_ACCESSOR   (RF,  16)
IMPLEMENT_EFLAG_ACCESSOR   (NT,  14)
IMPLEMENT_EFLAG_ACCESSOR_IOPL(   12)
IMPLEMENT_EFLAG_ACCESSOR   (IF,   9)
IMPLEMENT_EFLAG_ACCESSOR   (TF,   8)*/
IMPLEMENT_EFLAG_ACCESSOR   (DF,  10)

/*IMPLEMENT_EFLAG_SET_ACCESSOR         (ID,  21)
IMPLEMENT_EFLAG_SET_ACCESSOR         (VIP, 20)
IMPLEMENT_EFLAG_SET_ACCESSOR         (VIF, 19)
#if BX_SUPPORT_ALIGNMENT_CHECK && BX_CPU_LEVEL >= 4
IMPLEMENT_EFLAG_SET_ACCESSOR_AC      (     18)
#else
IMPLEMENT_EFLAG_SET_ACCESSOR         (AC,  18)
#endif
IMPLEMENT_EFLAG_SET_ACCESSOR_VM      (     17)
IMPLEMENT_EFLAG_SET_ACCESSOR_IF_RF_TF(RF,  16)
IMPLEMENT_EFLAG_SET_ACCESSOR         (NT,  14)
IMPLEMENT_EFLAG_SET_ACCESSOR         (DF,  10)
IMPLEMENT_EFLAG_SET_ACCESSOR_IF_RF_TF(IF,   9)
IMPLEMENT_EFLAG_SET_ACCESSOR_IF_RF_TF(TF,   8)*/
IMPLEMENT_EFLAG_SET_ACCESSOR         (DF,  10)

// <TAG-DEFINES-DECODE-START>

//
// For decoding...
//

// If the BxImmediate mask is set, the lowest 4 bits of the attribute
// specify which kinds of immediate data a required by instruction.

#define BxImmediate         0x000f // bits 3..0: any immediate
#define BxImmediate_I1      0x0001 // imm8 = 1
#define BxImmediate_Ib      0x0002 // 8 bit
#define BxImmediate_Ib_SE   0x0003 // sign extend to OS size
#define BxImmediate_Iw      0x0004 // 16 bit
#define BxImmediate_IbIb    0x0005 // SSE4A
#define BxImmediate_IwIb    0x0006 // enter_IwIb
#define BxImmediate_IwIw    0x0007 // call_Ap, not encodable in 64-bit mode
#define BxImmediate_IdIw    0x0008 // call_Ap, not encodable in 64-bit mode
#define BxImmediate_Id      0x0009 // 32 bit
#define BxImmediate_O       0x000A // MOV_ALOd, mov_OdAL, mov_eAXOv, mov_OveAX
#define BxImmediate_BrOff8  0x000B // Relative branch offset byte
#if BX_SUPPORT_X86_64
#define BxImmediate_Iq      0x000C // 64 bit override
#endif

#define BxImmediate_BrOff16 BxImmediate_Iw // Relative branch offset word, not encodable in 64-bit mode
#define BxImmediate_BrOff32 BxImmediate_Id // Relative branch offset dword

// Lookup for opcode and attributes in another opcode tables
// Totally 15 opcode groups supported
#define BxGroupX            0x00f0 // bits 7..4: opcode groups definition
#define BxGroupN            0x0010 // Group encoding: 0001
#define BxPrefixSSE         0x0020 // Group encoding: 0010
#define BxPrefixSSE66       0x0030 // Group encoding: 0011
#define BxFPEscape          0x0040 // Group encoding: 0100
#define BxRMGroup           0x0050 // Group encoding: 0101
#define Bx3ByteOp           0x0060 // Group encoding: 0110
#define BxOSizeGrp          0x0070 // Group encoding: 0111

#define BxLockable          0x0100 // bit 8
#define BxArithDstRM        0x0200 // bit 9

#if BX_SUPPORT_TRACE_CACHE
  #define BxTraceEnd        0x0400 // bit 10
#else
  #define BxTraceEnd        0
#endif

#ifdef BX_TRACE_CACHE_NO_SPECULATIVE_TRACING
  #define BxTraceJCC      BxTraceEnd
#else
  #define BxTraceJCC      0
#endif

#define BxGroup1          BxGroupN
#define BxGroup1A         BxGroupN
#define BxGroup2          BxGroupN
#define BxGroup3          BxGroupN
#define BxGroup4          BxGroupN
#define BxGroup5          BxGroupN
#define BxGroup6          BxGroupN
#define BxGroup7          BxGroupN
#define BxGroup8          BxGroupN
#define BxGroup9          BxGroupN

#define BxGroup11         BxGroupN
#define BxGroup12         BxGroupN
#define BxGroup13         BxGroupN
#define BxGroup14         BxGroupN
#define BxGroup15         BxGroupN
#define BxGroup16         BxGroupN

#define BxGroupFP         BxGroupN

// <TAG-DEFINES-DECODE-END>

#define setEFlagsOSZAPC(flags32) {                                      \
  BX_CPU_THIS_PTR eflags = (BX_CPU_THIS_PTR eflags & ~EFlagsOSZAPCMask) \
    | (flags32 & EFlagsOSZAPCMask);                                     \
  BX_CPU_THIS_PTR lf_flags_status = 0;                                  \
}

#define ASSERT_FLAGS_OxxxxC() {                                         \
  BX_CPU_THIS_PTR eflags |= (EFlagsOFMask | EFlagsCFMask);              \
  BX_CPU_THIS_PTR lf_flags_status &= ~(EFlagsOFMask | EFlagsCFMask);    \
}

#define SET_FLAGS_OxxxxC(new_of, new_cf) {                              \
  BX_CPU_THIS_PTR eflags &= ~((EFlagsOFMask | EFlagsCFMask));           \
  BX_CPU_THIS_PTR eflags |= ((new_of)<<11) | (new_cf);                  \
  BX_CPU_THIS_PTR lf_flags_status &= ~((EFlagsOFMask | EFlagsCFMask));  \
}

//#if defined(NEED_CPU_REG_SHORTCUTS)

#include "cpu/stack.h"

#define RSP_SPECULATIVE {              \
  BX_CPU_THIS_PTR speculative_rsp = 1; \
  BX_CPU_THIS_PTR prev_rsp = RSP;      \
}

#define RSP_COMMIT {                   \
  BX_CPU_THIS_PTR speculative_rsp = 0; \
}

//#endif // defined(NEED_CPU_REG_SHORTCUTS)

#endif
