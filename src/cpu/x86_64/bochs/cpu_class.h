/////////////////////////////////////////////////////////////////////////
// $Id: ,v 1.20 2009/01/16 18:18:58 sshwarts Exp $
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

#ifndef _ADVANCEDSS_X8664_CLASS_PROCESSOR_BOCHS_HEADER_
#define _ADVANCEDSS_X8664_CLASS_PROCESSOR_BOCHS_HEADER_

protected:

  // General register set
  // rax: accumulator
  // rbx: base
  // rcx: count
  // rdx: data
  // rbp: base pointer
  // rsi: source index
  // rdi: destination index
  // esp: stack pointer
  // r8..r15 x86-64 extended registers
  // rip: instruction pointer
  // nil: null register
  // tmp: temp register
  bx_gen_reg_t gen_reg[BX_GENERAL_REGISTERS+3];

  /* 31|30|29|28| 27|26|25|24| 23|22|21|20| 19|18|17|16
   * ==|==|=====| ==|==|==|==| ==|==|==|==| ==|==|==|==
   *  0| 0| 0| 0|  0| 0| 0| 0|  0| 0|ID|VP| VF|AC|VM|RF
   *
   * 15|14|13|12| 11|10| 9| 8|  7| 6| 5| 4|  3| 2| 1| 0
   * ==|==|=====| ==|==|==|==| ==|==|==|==| ==|==|==|==
   *  0|NT| IOPL| OF|DF|IF|TF| SF|ZF| 0|AF|  0|PF| 1|CF
   */
  Bit32u eflags; // Raw 32-bit value in x86 bit position.

  // status and control flags register set
  Bit32u lf_flags_status;
  bx_lf_flags_entry oszapc;

  volatile bx_address prev_rsp;
  volatile bx_bool    speculative_rsp;

  volatile uint64_t prev_rip;

	jmp_buf longjmp_env;
	volatile bochs_exec_status_t *bochs_exec_status;

#if BX_SUPPORT_FPU || BX_SUPPORT_MMX
  i387_t the_i387;
#endif

#if BX_SUPPORT_SSE
  bx_xmm_reg_t xmm[BX_XMM_REGISTERS]; // need TMP XMM register ?
  bx_mxcsr_t mxcsr;
#endif

  struct {
    bx_address rm_addr;       // The address offset after resolution
//    bx_phy_address paddress1; // physical address after translation of 1st len1 bytes of data
//    bx_phy_address paddress2; // physical address after translation of 2nd len2 bytes of data
//    Bit32u len1;              // Number of bytes in page 1
//    Bit32u len2;              // Number of bytes in page 2
//    bx_ptr_equiv_t pages;     // Number of pages access spans (1 or 2).  Also used
                              // for the case when a native host pointer is
                              // available for the R-M-W instructions.  The host
                              // pointer is stuffed here.  Since this field has
                              // to be checked anyways (and thus cached), if it
                              // is greated than 2 (the maximum possible for
                              // normal cases) it is a native pointer and is used
                              // for a direct write access.
  } address_xlation;

public:
	void bochs_exec(volatile bochs_exec_status_t *status);
	void exception(exception_code_t vector, Bit16u error_code, unsigned unused);

  BX_SMF void setEFlags(Bit32u val) BX_CPP_AttrRegparmN(1);
  BX_SMF void writeEFlags(Bit32u eflags, Bit32u changeMask) BX_CPP_AttrRegparmN(2); // Newer variant.

#if BX_SUPPORT_FPU || BX_SUPPORT_SSE >= 1
  BX_SMF void write_eflags_fpu_compare(int float_relation);
#endif

  BX_SMF Bit32u force_flags(void);
  BX_SMF Bit32u read_eflags(void) { return BX_CPU_THIS_PTR force_flags(); }

  BX_SMF Bit8u read_virtual_byte_32(unsigned seg, Bit32u offset) BX_CPP_AttrRegparmN(2);
  BX_SMF Bit16u read_virtual_word_32(unsigned seg, Bit32u offset) BX_CPP_AttrRegparmN(2);
  BX_SMF Bit32u read_virtual_dword_32(unsigned seg, Bit32u offset) BX_CPP_AttrRegparmN(2);
  BX_SMF Bit64u read_virtual_qword_32(unsigned seg, Bit32u offset) BX_CPP_AttrRegparmN(2);
#if BX_CPU_LEVEL >= 6
  BX_SMF void read_virtual_dqword_32(unsigned seg, Bit32u off, BxPackedXmmRegister *data) BX_CPP_AttrRegparmN(3);
  BX_SMF void read_virtual_dqword_aligned_32(unsigned seg, Bit32u off, BxPackedXmmRegister *data) BX_CPP_AttrRegparmN(3);
#endif

  BX_SMF void write_virtual_byte_32(unsigned seg, Bit32u offset, Bit8u data) BX_CPP_AttrRegparmN(3);
  BX_SMF void write_virtual_word_32(unsigned seg, Bit32u offset, Bit16u data) BX_CPP_AttrRegparmN(3);
  BX_SMF void write_virtual_dword_32(unsigned seg, Bit32u offset, Bit32u data) BX_CPP_AttrRegparmN(3);
  BX_SMF void write_virtual_qword_32(unsigned seg, Bit32u offset, Bit64u data) BX_CPP_AttrRegparmN(3);
#if BX_CPU_LEVEL >= 6
  BX_SMF void write_virtual_dqword_32(unsigned seg, Bit32u offset, const BxPackedXmmRegister *data) BX_CPP_AttrRegparmN(3);
  BX_SMF void write_virtual_dqword_aligned_32(unsigned seg, Bit32u offset, const BxPackedXmmRegister *data) BX_CPP_AttrRegparmN(3);
#endif

  BX_SMF Bit8u read_RMW_virtual_byte_32(unsigned seg, Bit32u offset) BX_CPP_AttrRegparmN(2);
  BX_SMF Bit16u read_RMW_virtual_word_32(unsigned seg, Bit32u offset) BX_CPP_AttrRegparmN(2);
  BX_SMF Bit32u read_RMW_virtual_dword_32(unsigned seg, Bit32u offset) BX_CPP_AttrRegparmN(2);
  BX_SMF Bit64u read_RMW_virtual_qword_32(unsigned seg, Bit32u offset) BX_CPP_AttrRegparmN(2);


#define ArithmeticalFlag(flag, lfMask, eflagsBitShift) \
  BX_SMF bx_bool get_##flag##Lazy(void); \
  BX_SMF bx_bool getB_##flag(void) { \
    if ((BX_CPU_THIS_PTR lf_flags_status & (lfMask)) == 0) \
      return (BX_CPU_THIS_PTR eflags >> eflagsBitShift) & 1; \
    else \
      return !!get_##flag##Lazy(); \
  } \
  BX_SMF bx_bool get_##flag(void) { \
    if ((BX_CPU_THIS_PTR lf_flags_status & (lfMask)) == 0) \
      return BX_CPU_THIS_PTR eflags & (lfMask); \
    else \
      return get_##flag##Lazy(); \
  } \
  BX_SMF void set_##flag(bx_bool val) { \
    BX_CPU_THIS_PTR lf_flags_status &= ~(lfMask); \
    BX_CPU_THIS_PTR eflags &= ~(lfMask); \
    BX_CPU_THIS_PTR eflags |= ((val)<<eflagsBitShift); \
  } \
  BX_SMF void clear_##flag(void) { \
    BX_CPU_THIS_PTR lf_flags_status &= ~(lfMask); \
    BX_CPU_THIS_PTR eflags &= ~(lfMask); \
  } \
  BX_SMF void assert_##flag(void) { \
    BX_CPU_THIS_PTR lf_flags_status &= ~(lfMask); \
    BX_CPU_THIS_PTR eflags |= (lfMask); \
  } \
  BX_SMF void force_##flag(void) { \
    if ((BX_CPU_THIS_PTR lf_flags_status & (lfMask)) != 0) { \
      set_##flag(!!get_##flag##Lazy()); \
    } \
  }

  ArithmeticalFlag(OF, EFlagsOFMask, 11);
  ArithmeticalFlag(SF, EFlagsSFMask,  7);
  ArithmeticalFlag(ZF, EFlagsZFMask,  6);
  ArithmeticalFlag(AF, EFlagsAFMask,  4);
  ArithmeticalFlag(PF, EFlagsPFMask,  2);
  ArithmeticalFlag(CF, EFlagsCFMask,  0);

  BX_SMF BX_CPP_INLINE void set_PF_base(Bit8u val);

/*  DECLARE_EFLAG_ACCESSOR   (ID,  21)
  DECLARE_EFLAG_ACCESSOR   (VIP, 20)
  DECLARE_EFLAG_ACCESSOR   (VIF, 19)
  DECLARE_EFLAG_ACCESSOR   (AC,  18)
  DECLARE_EFLAG_ACCESSOR   (VM,  17)
  DECLARE_EFLAG_ACCESSOR   (RF,  16)
  DECLARE_EFLAG_ACCESSOR   (NT,  14)
  DECLARE_EFLAG_ACCESSOR_IOPL(   12)
  DECLARE_EFLAG_ACCESSOR   (IF,   9)
  DECLARE_EFLAG_ACCESSOR   (TF,   8)*/
  DECLARE_EFLAG_ACCESSOR   (DF,  10)

#if BX_SUPPORT_FPU
  BX_SMF void print_state_FPU(void);
  BX_SMF void prepareFPU(bxInstruction_c *i, bx_bool = 1);
  BX_SMF void FPU_check_pending_exceptions(void);
  BX_SMF void FPU_update_last_instruction(bxInstruction_c *i);
  BX_SMF void FPU_stack_underflow(int stnr, int pop_stack = 0);
  BX_SMF void FPU_stack_overflow(void);
  BX_SMF unsigned FPU_exception(unsigned exception, bx_bool = 0);
  BX_SMF bx_address fpu_save_environment(bxInstruction_c *i);
  BX_SMF bx_address fpu_load_environment(bxInstruction_c *i);
  BX_SMF Bit8u pack_FPU_TW(Bit16u tag_word);
  BX_SMF Bit16u unpack_FPU_TW(Bit16u tag_byte);
#endif

#if BX_SUPPORT_MMX || BX_SUPPORT_SSE
  BX_SMF void prepareMMX(void);
  BX_SMF void prepareFPU2MMX(void); /* cause transition from FPU to MMX technology state */
  BX_SMF void print_state_MMX(void);
#endif

#if BX_SUPPORT_SSE
  BX_SMF void prepareSSE(void);
  BX_SMF void check_exceptionsSSE(int);
  BX_SMF void print_state_SSE(void);
#endif
  
  inline Bit64u get_laddr64(unsigned seg, Bit64u offset) {
    return offset;
  }

	#if BX_LITTLE_ENDIAN==0
		#error bochs fake io BX_INP and BX_OUTP does not support big endian
	#endif

	inline uint32_t BX_INP(uint16_t port, uint32_t bytes)
	{
		uint32_t data;
		this->io_space_read(&data, bytes, port);
		return data;
	}

	inline void BX_OUTP(uint16_t port, uint32_t data, uint32_t bytes)
	{
		this->io_space_write(&data, bytes, port);
	}

  BX_SMF void branch_near64(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF unsigned fetchDecode64(const Bit8u *fetchPtr, bxInstruction_c *i, unsigned remainingInPage) BX_CPP_AttrRegparmN(3);

  BX_SMF void    push_16(Bit16u value16) BX_CPP_AttrRegparmN(1);
  BX_SMF void    push_32(Bit32u value32) BX_CPP_AttrRegparmN(1);
  BX_SMF Bit16u  pop_16(void);
  BX_SMF Bit32u  pop_32(void);
  BX_SMF void    push_64(Bit64u value64) BX_CPP_AttrRegparmN(1);
  BX_SMF Bit64u  pop_64(void);

  BX_SMF void repeat(bxInstruction_c *i, BxExecutePtr_tR execute) BX_CPP_AttrRegparmN(2);
  BX_SMF void repeat_ZF(bxInstruction_c *i, BxExecutePtr_tR execute) BX_CPP_AttrRegparmN(2);

// <TAG-CLASS-CPU-START>

  // prototypes for CPU instructions...
  BX_SMF void ADD_ALIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OR_ALIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ADC_ALIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SBB_ALIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void AND_ALIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SUB_ALIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XOR_ALIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMP_ALIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void ADD_AXIw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OR_AXIw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ADC_AXIw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SBB_AXIw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void AND_AXIw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SUB_AXIw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XOR_AXIw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMP_AXIw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void ADD_EAXId(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OR_EAXId(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ADC_EAXId(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SBB_EAXId(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void AND_EAXId(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SUB_EAXId(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XOR_EAXId(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMP_EAXId(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void PUSH16_CS(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PUSH16_DS(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void POP16_DS(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PUSH16_ES(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void POP16_ES(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PUSH16_FS(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void POP16_FS(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PUSH16_GS(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void POP16_GS(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PUSH16_SS(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void POP16_SS(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void PUSH32_CS(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PUSH32_DS(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void POP32_DS(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PUSH32_ES(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void POP32_ES(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PUSH32_FS(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void POP32_FS(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PUSH32_GS(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void POP32_GS(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PUSH32_SS(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void POP32_SS(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void DAA(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void DAS(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void AAA(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void AAS(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void AAM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void AAD(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void PUSHAD32(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PUSHAD16(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void POPAD32(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void POPAD16(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ARPL_EwGw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PUSH_Id(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PUSH_Iw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void INSB32_YbDX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void INSB16_YbDX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void INSW32_YwDX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void INSW16_YwDX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void INSD32_YdDX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void INSD16_YdDX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OUTSB32_DXXb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OUTSB16_DXXb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OUTSW32_DXXw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OUTSW16_DXXw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OUTSD32_DXXd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OUTSD16_DXXd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void REP_INSB_YbDX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void REP_INSW_YwDX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void REP_INSD_YdDX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void REP_OUTSB_DXXb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void REP_OUTSW_DXXw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void REP_OUTSD_DXXd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void BOUND_GwMa(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BOUND_GdMa(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void TEST_EbGbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void TEST_EwGwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void TEST_EdGdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void XCHG_EbGbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XCHG_EwGwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XCHG_EdGdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void XCHG_EbGbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XCHG_EwGwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XCHG_EdGdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void MOV_EbGbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_EwGwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_GbEbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_GbEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_GwEwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void MOV32_GdEdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV32_EdGdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void MOV_EwSwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_EwSwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_SwEw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void LEA_GdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LEA_GwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void CBW(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CWD(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CALL32_Ap(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CALL16_Ap(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PUSHF_Fw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void POPF_Fw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PUSHF_Fd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void POPF_Fd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SAHF(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LAHF(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void MOV_ALOd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_EAXOd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_AXOd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_OdAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_OdEAX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_OdAX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void TEST_ALIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void TEST_EAXId(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void TEST_AXIw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  // repeatable instructions
  BX_SMF void REP_MOVSB_XbYb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void REP_MOVSW_XwYw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void REP_MOVSD_XdYd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void REP_CMPSB_XbYb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void REP_CMPSW_XwYw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void REP_CMPSD_XdYd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void REP_STOSB_YbAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void REP_LODSB_ALXb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void REP_SCASB_ALXb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void REP_STOSW_YwAX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void REP_LODSW_AXXw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void REP_SCASW_AXXw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void REP_STOSD_YdEAX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void REP_LODSD_EAXXd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void REP_SCASD_EAXXd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  // qualified by address size
  BX_SMF void CMPSB16_XbYb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMPSW16_XwYw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMPSD16_XdYd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMPSB32_XbYb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMPSW32_XwYw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMPSD32_XdYd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void SCASB16_ALXb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SCASW16_AXXw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SCASD16_EAXXd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SCASB32_ALXb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SCASW32_AXXw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SCASD32_EAXXd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void LODSB16_ALXb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LODSW16_AXXw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LODSD16_EAXXd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LODSB32_ALXb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LODSW32_AXXw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LODSD32_EAXXd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void STOSB16_YbAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void STOSW16_YwAX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void STOSD16_YdEAX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void STOSB32_YbAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void STOSW32_YwAX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void STOSD32_YdEAX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void MOVSB16_XbYb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVSW16_XwYw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVSD16_XdYd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVSB32_XbYb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVSW32_XwYw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVSD32_XdYd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void MOV_EdIdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_EwIwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_EbIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void ENTER16_IwIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ENTER32_IwIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LEAVE16(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LEAVE32(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void INT1(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void INT3(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void INT_Ib(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void INTO(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void IRET32(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void IRET16(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void SALC(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XLAT(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void LOOPNE16_Jb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LOOPE16_Jb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LOOP16_Jb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LOOPNE32_Jb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LOOPE32_Jb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LOOP32_Jb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JCXZ_Jb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JECXZ_Jb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void IN_ALIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void IN_AXIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void IN_EAXIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OUT_IbAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OUT_IbAX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OUT_IbEAX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CALL_Jw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CALL_Jd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JMP_Jd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JMP_Jw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JMP_Ap(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void IN_ALDX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void IN_AXDX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void IN_EAXDX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OUT_DXAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OUT_DXAX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OUT_DXEAX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void HLT(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMC(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CLC(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void STC(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CLI(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void STI(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CLD(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void STD(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void LAR_GvEw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LSL_GvEw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CLTS(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void INVD(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void WBINVD(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CLFLUSH(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void MOV_CdRd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_DdRd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_RdCd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_RdDd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_TdRd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_RdTd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void JO_Jw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JNO_Jw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JB_Jw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JNB_Jw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JZ_Jw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JNZ_Jw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JBE_Jw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JNBE_Jw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JS_Jw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JNS_Jw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JP_Jw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JNP_Jw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JL_Jw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JNL_Jw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JLE_Jw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JNLE_Jw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void JO_Jd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JNO_Jd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JB_Jd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JNB_Jd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JZ_Jd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JNZ_Jd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JBE_Jd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JNBE_Jd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JS_Jd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JNS_Jd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JP_Jd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JNP_Jd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JL_Jd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JNL_Jd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JLE_Jd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JNLE_Jd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void SETO_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SETNO_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SETB_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SETNB_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SETZ_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SETNZ_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SETBE_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SETNBE_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SETS_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SETNS_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SETP_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SETNP_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SETL_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SETNL_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SETLE_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SETNLE_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void SETO_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SETNO_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SETB_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SETNB_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SETZ_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SETNZ_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SETBE_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SETNBE_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SETS_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SETNS_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SETP_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SETNP_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SETL_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SETNL_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SETLE_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SETNLE_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void CPUID(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void SHRD_EwGwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SHRD_EwGwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SHLD_EwGwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SHLD_EwGwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SHRD_EdGdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SHRD_EdGdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SHLD_EdGdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SHLD_EdGdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void BSF_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BSF_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BSR_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BSR_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void BT_EwGwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BT_EdGdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTS_EwGwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTS_EdGdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTR_EwGwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTR_EdGdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTC_EwGwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTC_EdGdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void BT_EwGwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BT_EdGdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTS_EwGwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTS_EdGdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTR_EwGwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTR_EdGdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTC_EwGwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTC_EdGdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void BT_EwIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BT_EdIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTS_EwIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTS_EdIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTR_EwIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTR_EdIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTC_EwIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTC_EdIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void BT_EwIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BT_EdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTS_EwIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTS_EdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTR_EwIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTR_EdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTC_EwIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTC_EdIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void LES_GwMp(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LDS_GwMp(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LSS_GwMp(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LFS_GwMp(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LGS_GwMp(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LES_GdMp(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LDS_GdMp(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LSS_GdMp(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LFS_GdMp(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LGS_GdMp(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void MOVZX_GwEbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVZX_GdEbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVZX_GdEwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVSX_GwEbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVSX_GdEbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVSX_GdEwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void MOVZX_GwEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVZX_GdEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVZX_GdEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVSX_GwEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVSX_GdEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVSX_GdEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void BSWAP_ERX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void ADD_GbEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OR_GbEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ADC_GbEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SBB_GbEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void AND_GbEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SUB_GbEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XOR_GbEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMP_GbEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void ADD_EbIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OR_EbIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ADC_EbIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SBB_EbIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void AND_EbIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SUB_EbIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XOR_EbIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMP_EbIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void ADD_EbIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OR_EbIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ADC_EbIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SBB_EbIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void AND_EbIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SUB_EbIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XOR_EbIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMP_EbIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void ADD_EbGbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OR_EbGbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ADC_EbGbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SBB_EbGbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void AND_EbGbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SUB_EbGbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XOR_EbGbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMP_EbGbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void ADD_EwIwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OR_EwIwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ADC_EwIwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SBB_EwIwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void AND_EwIwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SUB_EwIwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XOR_EwIwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMP_EwIwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void ADD_EwIwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OR_EwIwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ADC_EwIwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SBB_EwIwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void AND_EwIwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SUB_EwIwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XOR_EwIwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMP_EwIwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void ADD_EdIdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OR_EdIdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ADC_EdIdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SBB_EdIdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void AND_EdIdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SUB_EdIdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XOR_EdIdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMP_EdIdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void ADD_EdIdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OR_EdIdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ADC_EdIdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SBB_EdIdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void AND_EdIdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SUB_EdIdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XOR_EdIdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMP_EdIdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void ADD_EwGwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OR_EwGwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ADC_EwGwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SBB_EwGwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void AND_EwGwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SUB_EwGwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XOR_EwGwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMP_EwGwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void ADD_EdGdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OR_EdGdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ADC_EdGdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SBB_EdGdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void AND_EdGdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SUB_EdGdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XOR_EdGdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMP_EdGdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void ADD_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OR_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ADC_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SBB_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void AND_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SUB_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XOR_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMP_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void ADD_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OR_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ADC_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SBB_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void AND_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SUB_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMP_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XOR_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void NOT_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void NOT_EwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void NOT_EdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void NOT_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void NOT_EwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void NOT_EdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void NEG_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void NEG_EwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void NEG_EdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void NEG_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void NEG_EwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void NEG_EdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void ROL_Eb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ROR_Eb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RCL_Eb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RCR_Eb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SHL_Eb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SHR_Eb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SAR_Eb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void ROL_Ew(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ROR_Ew(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RCL_Ew(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RCR_Ew(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SHL_Ew(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SHR_Ew(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SAR_Ew(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void ROL_EdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ROR_EdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RCL_EdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RCR_EdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SHL_EdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SHR_EdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SAR_EdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void ROL_EdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ROR_EdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RCL_EdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RCR_EdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SHL_EdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SHR_EdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SAR_EdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void TEST_EbIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void TEST_EwIwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void TEST_EdIdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void IMUL_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void IMUL_GdEdIdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void MUL_ALEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void IMUL_ALEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void DIV_ALEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void IDIV_ALEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void MUL_EAXEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void IMUL_EAXEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void DIV_EAXEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void IDIV_EAXEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void INC_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void DEC_EbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void INC_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void INC_EwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void INC_EdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void DEC_EbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void DEC_EwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void DEC_EdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void CALL_EdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CALL_EwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void CALL32_Ep(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CALL16_Ep(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JMP32_Ep(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JMP16_Ep(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void JMP_EdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JMP_EwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void SLDT_Ew(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void STR_Ew(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LLDT_Ew(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LTR_Ew(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VERR_Ew(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VERW_Ew(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void SGDT_Ms(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SIDT_Ms(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LGDT_Ms(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LIDT_Ms(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SMSW_EwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SMSW_EwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LMSW_Ew(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  // LOAD methods
  BX_SMF void LOAD_Eb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LOAD_Ew(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LOAD_Ed(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#if BX_SUPPORT_X86_64
  BX_SMF void LOAD_Eq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#endif

#if BX_SUPPORT_FPU == 0	// if FPU is disabled
  BX_SMF void FPU_ESC(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#endif

  BX_SMF void FWAIT(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

#if BX_SUPPORT_FPU
  // load/store
  BX_SMF void FLD_STi(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FLD_SINGLE_REAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FLD_DOUBLE_REAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FLD_EXTENDED_REAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FILD_WORD_INTEGER(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FILD_DWORD_INTEGER(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FILD_QWORD_INTEGER(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FBLD_PACKED_BCD(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void FST_STi(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FST_SINGLE_REAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FST_DOUBLE_REAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FSTP_EXTENDED_REAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FIST_WORD_INTEGER(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FIST_DWORD_INTEGER(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FISTP_QWORD_INTEGER(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FBSTP_PACKED_BCD(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void FISTTP16(bxInstruction_c *) BX_CPP_AttrRegparmN(1); // SSE3
  BX_SMF void FISTTP32(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FISTTP64(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  // control
  BX_SMF void FNINIT(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FNCLEX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void FRSTOR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FNSAVE(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FLDENV(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FNSTENV(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void FLDCW(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FNSTCW(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FNSTSW(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FNSTSW_AX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  // const
  BX_SMF void FLD1(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FLDL2T(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FLDL2E(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FLDPI(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FLDLG2(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FLDLN2(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FLDZ(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  // add
  BX_SMF void FADD_ST0_STj(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FADD_STi_ST0(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FADD_SINGLE_REAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FADD_DOUBLE_REAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FIADD_WORD_INTEGER(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FIADD_DWORD_INTEGER(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  // mul
  BX_SMF void FMUL_ST0_STj(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FMUL_STi_ST0(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FMUL_SINGLE_REAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FMUL_DOUBLE_REAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FIMUL_WORD_INTEGER (bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FIMUL_DWORD_INTEGER(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  // sub
  BX_SMF void FSUB_ST0_STj(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FSUBR_ST0_STj(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FSUB_STi_ST0(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FSUBR_STi_ST0(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FSUB_SINGLE_REAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FSUBR_SINGLE_REAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FSUB_DOUBLE_REAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FSUBR_DOUBLE_REAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void FISUB_WORD_INTEGER(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FISUBR_WORD_INTEGER(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FISUB_DWORD_INTEGER(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FISUBR_DWORD_INTEGER(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  // div
  BX_SMF void FDIV_ST0_STj(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FDIVR_ST0_STj(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FDIV_STi_ST0(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FDIVR_STi_ST0(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FDIV_SINGLE_REAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FDIVR_SINGLE_REAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FDIV_DOUBLE_REAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FDIVR_DOUBLE_REAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void FIDIV_WORD_INTEGER(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FIDIVR_WORD_INTEGER(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FIDIV_DWORD_INTEGER(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FIDIVR_DWORD_INTEGER(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  // compare
  BX_SMF void FCOM_STi(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FUCOM_STi(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FCOMI_ST0_STj(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FUCOMI_ST0_STj(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FCOM_SINGLE_REAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FCOM_DOUBLE_REAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FICOM_WORD_INTEGER(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FICOM_DWORD_INTEGER(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FCMOV_ST0_STj(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void FCOMPP(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FUCOMPP(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  // misc
  BX_SMF void FXCH_STi(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FNOP(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FPLEGACY(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FCHS(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FABS(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FTST(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FXAM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FDECSTP(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FINCSTP(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FFREE_STi(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FFREEP_STi(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void F2XM1(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FYL2X(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FPTAN(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FPATAN(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FXTRACT(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FPREM1(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FPREM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FYL2XP1(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FSQRT(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FSINCOS(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FRNDINT(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#undef FSCALE            // <sys/param.h> is #included on Mac OS X from bochs.h
  BX_SMF void FSCALE(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FSIN(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FCOS(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#endif

  /* MMX */
  BX_SMF void PUNPCKLBW_PqQd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PUNPCKLWD_PqQd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PUNPCKLDQ_PqQd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PACKSSWB_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PCMPGTB_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PCMPGTW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PCMPGTD_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PACKUSWB_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PUNPCKHBW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PUNPCKHWD_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PUNPCKHDQ_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PACKSSDW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVD_PqEd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVQ_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PCMPEQB_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PCMPEQW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PCMPEQD_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void EMMS(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVD_EdPd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVQ_QqPq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSRLW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSRLD_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSRLQ_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMULLW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSUBUSB_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSUBUSW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PAND_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PADDUSB_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PADDUSW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PANDN_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSRAW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSRAD_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMULHW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSUBSB_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSUBSW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void POR_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PADDSB_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PADDSW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PXOR_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSLLW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSLLD_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSLLQ_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMADDWD_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSUBB_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSUBW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSUBD_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PADDB_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PADDW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PADDD_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSRLW_PqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSRAW_PqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSLLW_PqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSRLD_PqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSRAD_PqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSLLD_PqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSRLQ_PqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSLLQ_PqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  /* MMX */

#if BX_SUPPORT_3DNOW
  BX_SMF void PFPNACC_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PI2FW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PI2FD_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PF2IW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PF2ID_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PFNACC_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PFCMPGE_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PFMIN_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PFRCP_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PFRSQRT_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PFSUB_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PFADD_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PFCMPGT_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PFMAX_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PFRCPIT1_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PFRSQIT1_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PFSUBR_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PFACC_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PFCMPEQ_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PFMUL_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PFRCPIT2_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMULHRW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSWAPD_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#endif

  /* SSE */
  BX_SMF void FXSAVE(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void FXRSTOR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LDMXCSR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void STMXCSR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PREFETCH(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  /* SSE */

  /* SSE */
  BX_SMF void ANDPS_VpsWps(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ORPS_VpsWps(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XORPS_VpsWps(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ANDNPS_VpsWps(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVUPS_VpsWps(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVSS_VssWss(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVUPS_WpsVps(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVSS_WssVss(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVLPS_VpsMq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVLPS_MqVps(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVHPS_VpsMq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVHPS_MqVps(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVAPS_VpsWps(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVAPS_WpsVps(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CVTPI2PS_VpsQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CVTSI2SS_VssEd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVNTPS_MpsVps(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CVTTPS2PI_PqWps(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CVTTSS2SI_GdWss(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CVTPS2PI_PqWps(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CVTSS2SI_GdWss(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void UCOMISS_VssWss(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void COMISS_VpsWps(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVMSKPS_GdVRps(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SQRTPS_VpsWps(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SQRTSS_VssWss(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RSQRTPS_VpsWps(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RSQRTSS_VssWss(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RCPPS_VpsWps(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RCPSS_VssWss(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ADDPS_VpsWps(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ADDSS_VssWss(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MULPS_VpsWps(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MULSS_VssWss(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SUBPS_VpsWps(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SUBSS_VssWss(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MINPS_VpsWps(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MINSS_VssWss(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void DIVPS_VpsWps(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void DIVSS_VssWss(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MAXPS_VpsWps(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MAXSS_VssWss(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSHUFW_PqQqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSHUFLW_VdqWdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMPPS_VpsWpsIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMPSS_VssWssIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PINSRW_PqEwIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PEXTRW_GdPqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SHUFPS_VpsWpsIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMOVMSKB_GdPRq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMINUB_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMAXUB_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PAVGB_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PAVGW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMULHUW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVNTQ_MqPq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMINSW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMAXSW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSADBW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MASKMOVQ_PqPRq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  /* SSE */

  /* SSE2 */
  BX_SMF void MOVSD_VsdWsd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVSD_WsdVsd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CVTPI2PD_VpdQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CVTSI2SD_VsdEd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CVTTPD2PI_PqWpd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CVTTSD2SI_GdWsd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CVTPD2PI_PqWpd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CVTSD2SI_GdWsd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void UCOMISD_VsdWsd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void COMISD_VpdWpd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVMSKPD_GdVRpd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SQRTPD_VpdWpd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SQRTSD_VsdWsd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ADDPD_VpdWpd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ADDSD_VsdWsd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MULPD_VpdWpd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MULSD_VsdWsd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CVTPS2PD_VpsWps(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CVTPD2PS_VpdWpd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CVTSD2SS_VsdWsd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CVTSS2SD_VssWss(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CVTDQ2PS_VpsWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CVTPS2DQ_VdqWps(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CVTTPS2DQ_VdqWps(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SUBPD_VpdWpd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SUBSD_VsdWsd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MINPD_VpdWpd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MINSD_VsdWsd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void DIVPD_VpdWpd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void DIVSD_VsdWsd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MAXPD_VpdWpd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MAXSD_VsdWsd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PUNPCKLBW_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PUNPCKLWD_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void UNPCKLPS_VpsWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PACKSSWB_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PCMPGTB_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PCMPGTW_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PCMPGTD_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PACKUSWB_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PUNPCKHBW_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PUNPCKHWD_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void UNPCKHPS_VpsWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PACKSSDW_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PUNPCKLQDQ_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PUNPCKHQDQ_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVD_VdqEd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSHUFD_VdqWdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSHUFHW_VdqWdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PCMPEQB_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PCMPEQW_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PCMPEQD_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVD_EdVd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVQ_VqWq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMPPD_VpdWpdIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMPSD_VsdWsdIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVNTI_MdGd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PINSRW_VdqEwIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PEXTRW_GdUdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SHUFPD_VpdWpdIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSRLW_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSRLD_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSRLQ_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PADDQ_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PADDQ_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMULLW_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVQ_WqVq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVDQ2Q_PqVRq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVQ2DQ_VdqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMOVMSKB_GdUdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSUBUSB_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSUBUSW_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMINUB_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PADDUSB_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PADDUSW_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMAXUB_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PAVGB_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSRAW_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSRAD_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PAVGW_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMULHUW_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMULHW_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CVTTPD2DQ_VqWpd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CVTPD2DQ_VqWpd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CVTDQ2PD_VpdWq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSUBSB_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSUBSW_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMINSW_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PADDSB_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PADDSW_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMAXSW_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSLLW_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSLLD_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSLLQ_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMULUDQ_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMULUDQ_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMADDWD_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSADBW_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MASKMOVDQU_VdqUdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSUBB_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSUBW_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSUBD_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSUBQ_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSUBQ_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PADDB_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PADDW_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PADDD_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSRLW_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSRAW_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSLLW_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSRLD_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSRAD_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSLLD_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSRLQ_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSRLDQ_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSLLQ_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSLLDQ_UdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  /* SSE2 */

  /* SSE3 */
  BX_SMF void MOVDDUP_VpdWq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVSLDUP_VpsWps(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVSHDUP_VpsWps(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void HADDPD_VpdWpd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void HADDPS_VpsWps(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void HSUBPD_VpdWpd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void HSUBPS_VpsWps(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ADDSUBPD_VpdWpd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ADDSUBPS_VpsWps(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LDDQU_VdqMdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  /* SSE3 */

  // 3-byte opcodes
#if (BX_SUPPORT_SSE >= 4) || (BX_SUPPORT_SSE >= 3 && BX_SUPPORT_SSE_EXTENSION > 0)
  /* SSE3E */
  BX_SMF void PSHUFB_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PHADDW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PHADDD_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PHADDSW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMADDUBSW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PHSUBSW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PHSUBW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PHSUBD_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSIGNB_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSIGNW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSIGND_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMULHRSW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PABSB_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PABSW_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PABSD_PqQq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PALIGNR_PqQqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void PSHUFB_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PHADDW_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PHADDD_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PHADDSW_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMADDUBSW_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PHSUBSW_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PHSUBW_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PHSUBD_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSIGNB_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSIGNW_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PSIGND_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMULHRSW_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PABSB_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PABSW_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PABSD_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PALIGNR_VdqWdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  /* SSE3E */

  /* SSE4.1 */
  BX_SMF void PBLENDVB_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BLENDVPS_VpsWps(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BLENDVPD_VpdWpd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PTEST_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMULDQ_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PCMPEQQ_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PACKUSDW_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMOVSXBW_VdqWq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMOVSXBD_VdqWd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMOVSXBQ_VdqWw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMOVSXWD_VdqWq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMOVSXWQ_VdqWd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMOVSXDQ_VdqWq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMOVZXBW_VdqWq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMOVZXBD_VdqWd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMOVZXBQ_VdqWw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMOVZXWD_VdqWq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMOVZXWQ_VdqWd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMOVZXDQ_VdqWq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMINSB_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMINSD_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMINUW_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMINUD_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMAXSB_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMAXSD_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMAXUW_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMAXUD_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PMULLD_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PHMINPOSUW_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ROUNDPS_VpsWpsIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ROUNDPD_VpdWpdIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ROUNDSS_VssWssIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ROUNDSD_VsdWsdIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BLENDPS_VpsWpsIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BLENDPD_VpdWpdIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PBLENDW_VdqWdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PEXTRB_HbdUdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PEXTRW_HwdUdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PEXTRD_HdUdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void EXTRACTPS_HdUpsIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PINSRB_VdqEbIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void INSERTPS_VpsWssIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PINSRD_VdqEdIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void DPPS_VpsWpsIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void DPPD_VpdWpdIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MPSADBW_VdqWdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVNTDQA_VdqMdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  /* SSE4.1 */

  /* SSE4.2 */
  BX_SMF void CRC32_GdEb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CRC32_GdEw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CRC32_GdEd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#if BX_SUPPORT_X86_64
  BX_SMF void CRC32_GdEq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#endif
  BX_SMF void PCMPGTQ_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PCMPESTRM_VdqWdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PCMPESTRI_VdqWdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PCMPISTRM_VdqWdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PCMPISTRI_VdqWdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  /* SSE4.2 */
#endif

  /* MOVBE Intel Atom(R) instruction */
  BX_SMF void MOVBE_GwEw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVBE_GdEd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVBE_EwGw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVBE_EdGd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#if BX_SUPPORT_X86_64
  BX_SMF void MOVBE_GqEq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVBE_EqGq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#endif

  /* XSAVE/XRSTOR extensions */
  BX_SMF void XSAVE(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XRSTOR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XGETBV(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XSETBV(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  /* XSAVE/XRSTOR extensions */

  /* AES instructions */
  BX_SMF void AESIMC_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void AESENC_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void AESENCLAST_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void AESDEC_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void AESDECLAST_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void AESKEYGENASSIST_VdqWdqIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  /* AES instructions */

  /* VMX instructions */
  BX_SMF void VMXON(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMXOFF(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMCALL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMLAUNCH(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMCLEAR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMPTRLD(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMPTRST(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMREAD(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void VMWRITE(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  /* VMX instructions */

  /*** Duplicate SSE instructions ***/
  // Although in implementation, these instructions are aliased to the
  // another function, it's nice to have them call a separate function when
  // the decoder is being tested in stand-alone mode.
#ifdef STAND_ALONE_DECODER
  BX_SMF void MOVUPD_VpdWpd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVUPD_WpdVpd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVAPD_VpdWpd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVAPD_WpdVpd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVDQU_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVDQU_WdqVdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVDQA_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVDQA_WdqVdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PUNPCKHDQ_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PUNPCKLDQ_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ANDPD_VpdWpd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ANDNPD_VpdWpd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ORPD_VpdWpd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XORPD_VpdWpd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PAND_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PANDN_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void POR_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PXOR_VdqWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void UNPCKHPD_VpdWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void UNPCKLPD_VpdWdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVLPD_VsdMq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVLPD_MqVsd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVHPD_VsdMq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVHPD_MqVsd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVNTPD_MdqVpd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVNTDQ_MdqVdq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#endif

#if BX_SUPPORT_SSE >= 2
  #define BX_SSE2_ALIAS(i) i
#else
  #define BX_SSE2_ALIAS(i) &BX_CPU_C::BxError
#endif

#if BX_SUPPORT_3DNOW
  #define BX_3DNOW_ALIAS(i) i
#else
  #define BX_3DNOW_ALIAS(i) &BX_CPU_C::BxError
#endif

  BX_SMF void CMPXCHG_XBTS(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMPXCHG_IBTS(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMPXCHG8B(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RETnear32_Iw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RETnear32(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RETnear16_Iw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RETnear16(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RETfar32_Iw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RETfar32(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RETfar16_Iw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RETfar16(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void XADD_EbGbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XADD_EwGwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XADD_EdGdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void XADD_EbGbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XADD_EwGwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XADD_EdGdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

#if BX_CPU_LEVEL == 2
  BX_SMF void LOADALL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#endif

  BX_SMF void CMOVO_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVNO_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVB_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVNB_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVZ_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVNZ_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVBE_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVNBE_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVS_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVNS_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVP_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVNP_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVL_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVNL_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVLE_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVNLE_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void CMOVO_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVNO_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVB_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVNB_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVZ_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVNZ_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVBE_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVNBE_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVS_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVNS_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVP_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVNP_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVL_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVNL_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVLE_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVNLE_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void CWDE(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CDQ(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void CMPXCHG_EbGbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMPXCHG_EwGwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMPXCHG_EdGdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void CMPXCHG_EbGbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMPXCHG_EwGwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMPXCHG_EdGdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void MUL_AXEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void IMUL_AXEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void DIV_AXEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void IDIV_AXEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void IMUL_GwEwIwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void IMUL_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void NOP(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PAUSE(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_RLIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_RHIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_RXIw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_ERXId(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void INC_RX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void DEC_RX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void INC_ERX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void DEC_ERX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XCHG_RXAX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XCHG_ERXEAX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void PUSH_RX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PUSH_EwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PUSH_ERX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PUSH_EdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void POP_RX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void POP_EwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void POP_ERX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void POP_EdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void POPCNT_GwEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void POPCNT_GdEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#if BX_SUPPORT_X86_64
  BX_SMF void POPCNT_GqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#endif

#if BX_SUPPORT_X86_64
  // 64 bit extensions
  BX_SMF void ADD_GqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OR_GqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ADC_GqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SBB_GqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void AND_GqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SUB_GqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XOR_GqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMP_GqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void ADD_RAXId(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OR_RAXId(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ADC_RAXId(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SBB_RAXId(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void AND_RAXId(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SUB_RAXId(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XOR_RAXId(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMP_RAXId(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void ADD_EqGqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OR_EqGqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ADC_EqGqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SBB_EqGqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void AND_EqGqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SUB_EqGqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XOR_EqGqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMP_EqGqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void ADD_EqIdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OR_EqIdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ADC_EqIdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SBB_EqIdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void AND_EqIdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SUB_EqIdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XOR_EqIdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMP_EqIdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void ADD_EqIdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OR_EqIdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ADC_EqIdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SBB_EqIdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void AND_EqIdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SUB_EqIdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XOR_EqIdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMP_EqIdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void TEST_EqGqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void TEST_RAXId(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void XCHG_EqGqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XCHG_EqGqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void LEA_GqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void MOV_RAXOq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_OqRAX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_EAXOq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_OqEAX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_AXOq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_OqAX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_ALOq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_OqAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void MOV_EqGqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_GqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_GqEqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_EqIdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_EqIdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  // repeatable instructions
  BX_SMF void REP_MOVSQ_XqYq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void REP_CMPSQ_XqYq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void REP_STOSQ_YqRAX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void REP_LODSQ_RAXXq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void REP_SCASQ_RAXXq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  // qualified by address size
  BX_SMF void CMPSB64_XbYb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMPSW64_XwYw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMPSD64_XdYd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SCASB64_ALXb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SCASW64_AXXw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SCASD64_EAXXd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LODSB64_ALXb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LODSW64_AXXw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LODSD64_EAXXd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void STOSB64_YbAL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void STOSW64_YwAX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void STOSD64_YdEAX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVSB64_XbYb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVSW64_XwYw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVSD64_XdYd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void CMPSQ32_XqYq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMPSQ64_XqYq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SCASQ32_RAXXq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SCASQ64_RAXXq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LODSQ32_RAXXq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LODSQ64_RAXXq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void STOSQ32_YqRAX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void STOSQ64_YqRAX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVSQ32_XqYq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVSQ64_XqYq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void INSB64_YbDX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void INSW64_YwDX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void INSD64_YdDX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void OUTSB64_DXXb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OUTSW64_DXXw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void OUTSD64_DXXd(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void CALL_Jq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JMP_Jq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void JO_Jq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JNO_Jq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JB_Jq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JNB_Jq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JZ_Jq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JNZ_Jq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JBE_Jq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JNBE_Jq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JS_Jq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JNS_Jq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JP_Jq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JNP_Jq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JL_Jq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JNL_Jq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JLE_Jq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JNLE_Jq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void ENTER64_IwIb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LEAVE64(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void IRET64(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void MOV_CqRq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_DqRq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_RqCq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV_RqDq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void SHLD_EqGqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SHLD_EqGqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SHRD_EqGqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SHRD_EqGqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void MOV64_GdEdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOV64_EdGdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void MOVZX_GqEbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVZX_GqEwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVSX_GqEbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVSX_GqEwM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVSX_GqEdM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void MOVZX_GqEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVZX_GqEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVSX_GqEbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVSX_GqEwR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVSX_GqEdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void BSF_GqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BSR_GqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void BT_EqGqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTS_EqGqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTR_EqGqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTC_EqGqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void BT_EqGqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTS_EqGqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTR_EqGqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTC_EqGqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void BT_EqIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTS_EqIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTR_EqIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTC_EqIbM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void BT_EqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTS_EqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTR_EqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BTC_EqIbR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void BSWAP_RRX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void ROL_EqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ROR_EqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RCL_EqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RCR_EqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SHL_EqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SHR_EqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SAR_EqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void ROL_EqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void ROR_EqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RCL_EqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RCR_EqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SHL_EqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SHR_EqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SAR_EqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void NOT_EqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void NEG_EqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void NOT_EqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void NEG_EqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void TEST_EqIdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void MUL_RAXEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void IMUL_RAXEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void DIV_RAXEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void IDIV_RAXEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void IMUL_GqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void IMUL_GqEqIdR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void INC_EqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void DEC_EqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void INC_EqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void DEC_EqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CALL_EqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CALL64_Ep(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JMP_EqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JMP64_Ep(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PUSHF_Fq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void POPF_Fq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void CMPXCHG_EqGqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMPXCHG_EqGqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void CDQE(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CQO(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void XADD_EqGqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XADD_EqGqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void RETnear64_Iw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RETnear64(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RETfar64_Iw(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void CMOVO_GqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVNO_GqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVB_GqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVNB_GqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVZ_GqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVNZ_GqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVBE_GqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVNBE_GqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVS_GqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVNS_GqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVP_GqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVNP_GqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVL_GqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVNL_GqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVLE_GqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMOVNLE_GqEqR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void MOV_RRXIq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PUSH_EqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PUSH_RRX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void POP_EqM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void POP_RRX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void XCHG_RRXRAX(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void PUSH64_Id(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PUSH64_FS(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void POP64_FS(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void PUSH64_GS(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void POP64_GS(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void LSS_GqMp(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LFS_GqMp(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LGS_GqMp(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void SGDT64_Ms(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SIDT64_Ms(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LGDT64_Ms(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LIDT64_Ms(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void SYSCALL(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SYSRET(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SWAPGS(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RDTSCP(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void CMPXCHG16B(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void LOOPNE64_Jb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LOOPE64_Jb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void LOOP64_Jb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void JRCXZ_Jb(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void MOVQ_EqPq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVQ_EqVq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVQ_PqEq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVQ_VdqEq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MOVNTI_MqGq(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
#endif  // #if BX_SUPPORT_X86_64

  BX_SMF void INVLPG(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RSM(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void WRMSR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RDTSC(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RDPMC(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void RDMSR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SYSENTER(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void SYSEXIT(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void MONITOR(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void MWAIT(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF void UndefinedOpcode(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF void BxError(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

  BX_SMF bx_address BxResolve32Base(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF bx_address BxResolve32BaseIndex(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF bx_address BxResolve64Base(bxInstruction_c *) BX_CPP_AttrRegparmN(1);
  BX_SMF bx_address BxResolve64BaseIndex(bxInstruction_c *) BX_CPP_AttrRegparmN(1);

// <TAG-CLASS-CPU-END>

#endif
