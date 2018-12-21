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

#ifndef _ADVANCEDSS_X8664_BOCHS_GENERAL_HEADER_
#define _ADVANCEDSS_X8664_BOCHS_GENERAL_HEADER_

#include "bochs.h"

#define BOCHS_MOD_INVALID_INSTRUCTION \
	exception(BX_UD_EXCEPTION, 0, 0);

#define BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE \
	SANITY_ASSERT(0)

#define IsCanonical(V) 1

// segment register encoding
#define BX_SEG_REG_ES    0
#define BX_SEG_REG_CS    1
#define BX_SEG_REG_SS    2
#define BX_SEG_REG_DS    3
#define BX_SEG_REG_FS    4
#define BX_SEG_REG_GS    5
// NULL now has to fit in 3 bits.
#define BX_SEG_REG_NULL  7
#define BX_NULL_SEG_REG(seg) ((seg) == BX_SEG_REG_NULL)

#define BX_8BIT_REG_AL 0
#define BX_8BIT_REG_CL 1

#define BX_16BIT_REG_AX 0
#define BX_16BIT_REG_CX 1
#define BX_16BIT_REG_DX 2
#define BX_16BIT_REG_BX 3
#define BX_16BIT_REG_SP 4
#define BX_16BIT_REG_BP 5
#define BX_16BIT_REG_SI 6
#define BX_16BIT_REG_DI 7

#define BX_32BIT_REG_EAX 0
#define BX_32BIT_REG_ECX 1
#define BX_32BIT_REG_EDX 2
#define BX_32BIT_REG_EBX 3
#define BX_32BIT_REG_ESP 4
#define BX_32BIT_REG_EBP 5
#define BX_32BIT_REG_ESI 6
#define BX_32BIT_REG_EDI 7

#define BX_64BIT_REG_RAX 0
#define BX_64BIT_REG_RCX 1
#define BX_64BIT_REG_RDX 2
#define BX_64BIT_REG_RBX 3
#define BX_64BIT_REG_RSP 4
#define BX_64BIT_REG_RBP 5
#define BX_64BIT_REG_RSI 6
#define BX_64BIT_REG_RDI 7

#define BX_64BIT_REG_R8  8
#define BX_64BIT_REG_R9  9
#define BX_64BIT_REG_R10 10
#define BX_64BIT_REG_R11 11
#define BX_64BIT_REG_R12 12
#define BX_64BIT_REG_R13 13
#define BX_64BIT_REG_R14 14
#define BX_64BIT_REG_R15 15

#define BX_GENERAL_REGISTERS 16

#define BX_16BIT_REG_IP  BX_GENERAL_REGISTERS
#define BX_32BIT_REG_EIP BX_GENERAL_REGISTERS
#define BX_64BIT_REG_RIP BX_GENERAL_REGISTERS

#define BX_NIL_REGISTER (BX_GENERAL_REGISTERS+1)

#define BX_TMP_REGISTER (BX_GENERAL_REGISTERS+2)

//#if defined(NEED_CPU_REG_SHORTCUTS)

/* WARNING:
   Only BX_CPU_C member functions can use these shortcuts safely!
   Functions that use the shortcuts outside of BX_CPU_C might work
   when BX_USE_CPU_SMF=1 but will fail when BX_USE_CPU_SMF=0
   (for example in SMP mode).
*/

// access to 8 bit general registers
#define AL (BX_CPU_THIS_PTR gen_reg[0].word.byte.rl)
#define CL (BX_CPU_THIS_PTR gen_reg[1].word.byte.rl)
#define DL (BX_CPU_THIS_PTR gen_reg[2].word.byte.rl)
#define BL (BX_CPU_THIS_PTR gen_reg[3].word.byte.rl)
#define AH (BX_CPU_THIS_PTR gen_reg[0].word.byte.rh)
#define CH (BX_CPU_THIS_PTR gen_reg[1].word.byte.rh)
#define DH (BX_CPU_THIS_PTR gen_reg[2].word.byte.rh)
#define BH (BX_CPU_THIS_PTR gen_reg[3].word.byte.rh)

#define TMP8L (BX_CPU_THIS_PTR gen_reg[BX_TMP_REGISTER].word.byte.rl)

// access to 16 bit general registers
#define AX (BX_CPU_THIS_PTR gen_reg[0].word.rx)
#define CX (BX_CPU_THIS_PTR gen_reg[1].word.rx)
#define DX (BX_CPU_THIS_PTR gen_reg[2].word.rx)
#define BX (BX_CPU_THIS_PTR gen_reg[3].word.rx)
#define SP (BX_CPU_THIS_PTR gen_reg[4].word.rx)
#define BP (BX_CPU_THIS_PTR gen_reg[5].word.rx)
#define SI (BX_CPU_THIS_PTR gen_reg[6].word.rx)
#define DI (BX_CPU_THIS_PTR gen_reg[7].word.rx)

// access to 16 bit instruction pointer
#define IP (BX_CPU_THIS_PTR gen_reg[BX_16BIT_REG_IP].word.rx)

#define TMP16 (BX_CPU_THIS_PTR gen_reg[BX_TMP_REGISTER].word.rx)

// accesss to 32 bit general registers
#define EAX (BX_CPU_THIS_PTR gen_reg[0].dword.erx)
#define ECX (BX_CPU_THIS_PTR gen_reg[1].dword.erx)
#define EDX (BX_CPU_THIS_PTR gen_reg[2].dword.erx)
#define EBX (BX_CPU_THIS_PTR gen_reg[3].dword.erx)
#define ESP (BX_CPU_THIS_PTR gen_reg[4].dword.erx)
#define EBP (BX_CPU_THIS_PTR gen_reg[5].dword.erx)
#define ESI (BX_CPU_THIS_PTR gen_reg[6].dword.erx)
#define EDI (BX_CPU_THIS_PTR gen_reg[7].dword.erx)

// access to 32 bit instruction pointer
#define EIP (BX_CPU_THIS_PTR gen_reg[BX_32BIT_REG_EIP].dword.erx)

#define TMP32 (BX_CPU_THIS_PTR gen_reg[BX_TMP_REGISTER].dword.erx)

// accesss to 64 bit general registers
#define RAX (BX_CPU_THIS_PTR gen_reg[0].rrx)
#define RCX (BX_CPU_THIS_PTR gen_reg[1].rrx)
#define RDX (BX_CPU_THIS_PTR gen_reg[2].rrx)
#define RBX (BX_CPU_THIS_PTR gen_reg[3].rrx)
#define RSP (BX_CPU_THIS_PTR gen_reg[4].rrx)
#define RBP (BX_CPU_THIS_PTR gen_reg[5].rrx)
#define RSI (BX_CPU_THIS_PTR gen_reg[6].rrx)
#define RDI (BX_CPU_THIS_PTR gen_reg[7].rrx)
#define R8  (BX_CPU_THIS_PTR gen_reg[8].rrx)
#define R9  (BX_CPU_THIS_PTR gen_reg[9].rrx)
#define R10 (BX_CPU_THIS_PTR gen_reg[10].rrx)
#define R11 (BX_CPU_THIS_PTR gen_reg[11].rrx)
#define R12 (BX_CPU_THIS_PTR gen_reg[12].rrx)
#define R13 (BX_CPU_THIS_PTR gen_reg[13].rrx)
#define R14 (BX_CPU_THIS_PTR gen_reg[14].rrx)
#define R15 (BX_CPU_THIS_PTR gen_reg[15].rrx)

// access to 64 bit instruction pointer
#define RIP (BX_CPU_THIS_PTR gen_reg[BX_64BIT_REG_RIP].rrx)

#define TMP64 (BX_CPU_THIS_PTR gen_reg[BX_TMP_REGISTER].rrx)

#define BX_READ_8BIT_REGx(index,extended)  ((((index) & 4) == 0 || (extended)) ? \
  (BX_CPU_THIS_PTR gen_reg[index].word.byte.rl) : \
  (BX_CPU_THIS_PTR gen_reg[(index)-4].word.byte.rh))
#define BX_READ_64BIT_REG(index) (BX_CPU_THIS_PTR gen_reg[index].rrx)

#define BX_READ_8BIT_REGH(index) (BX_CPU_THIS_PTR gen_reg[index].word.byte.rh)
#define BX_READ_16BIT_REG(index) (BX_CPU_THIS_PTR gen_reg[index].word.rx)
#define BX_READ_32BIT_REG(index) (BX_CPU_THIS_PTR gen_reg[index].dword.erx)

#define BX_WRITE_8BIT_REGH(index, val) {\
  BX_CPU_THIS_PTR gen_reg[index].word.byte.rh = val; \
}

#define BX_WRITE_16BIT_REG(index, val) {\
  BX_CPU_THIS_PTR gen_reg[index].word.rx = val; \
}

#define BX_WRITE_8BIT_REGx(index, extended, val) {\
  if (((index) & 4) == 0 || (extended)) \
    BX_CPU_THIS_PTR gen_reg[index].word.byte.rl = val; \
  else \
    BX_CPU_THIS_PTR gen_reg[(index)-4].word.byte.rh = val; \
}

#define BX_WRITE_32BIT_REGZ(index, val) {\
  BX_CPU_THIS_PTR gen_reg[index].rrx = (Bit32u) val; \
}

#define BX_WRITE_64BIT_REG(index, val) {\
  BX_CPU_THIS_PTR gen_reg[index].rrx = val; \
}
#define BX_CLEAR_64BIT_HIGH(index) {\
  BX_CPU_THIS_PTR gen_reg[index].dword.hrx = 0; \
}

//#endif  // defined(NEED_CPU_REG_SHORTCUTS)

struct BxExceptionInfo {
  unsigned exception_type;
  unsigned exception_class;
  bx_bool push_error;
};

#define BX_DE_EXCEPTION   0 // Divide Error (fault)
#define BX_DB_EXCEPTION   1 // Debug (fault/trap)
#define BX_BP_EXCEPTION   3 // Breakpoint (trap)
#define BX_OF_EXCEPTION   4 // Overflow (trap)
#define BX_BR_EXCEPTION   5 // BOUND (fault)
#define BX_UD_EXCEPTION   6
#define BX_NM_EXCEPTION   7
#define BX_DF_EXCEPTION   8
#define BX_TS_EXCEPTION  10
#define BX_NP_EXCEPTION  11
#define BX_SS_EXCEPTION  12
#define BX_GP_EXCEPTION  13
#define BX_PF_EXCEPTION  14
#define BX_MF_EXCEPTION  16
#define BX_AC_EXCEPTION  17
#define BX_MC_EXCEPTION  18
#define BX_XM_EXCEPTION  19

#define BX_CPU_HANDLED_EXCEPTIONS  20

#  define BX_CPU_THIS_PTR  this->
#  define BX_CPU_THIS      this
#  define BX_SMF
#  define BX_CPU_C_PREFIX  BX_CPU_C::
// with normal member functions, calling a member fn pointer looks like
// object->*(fnptr)(arg, ...);
// Since this is different from when SMF=1, encapsulate it in a macro.
#  define BX_CPU_CALL_METHOD(func, args) \
            (this->*((BxExecutePtr_tR) (func))) args
#  define BX_CPU_CALL_METHODR(func, args) \
            (this->*((BxResolvePtr_tR) (func))) args
#  define BX_CPU_CALL_METHODR_(OBJ, func, args) \
            ((OBJ)->*((BxResolvePtr_tR) (func))) args

// accessors for all eflags in bx_flags_reg_t
// The macro is used once for each flag bit
// Do not use for arithmetic flags !
#define DECLARE_EFLAG_ACCESSOR(name,bitnum)                     \
  BX_SMF BX_CPP_INLINE Bit32u  get_##name ();                   \
  BX_SMF BX_CPP_INLINE bx_bool getB_##name ();                  \
  BX_SMF BX_CPP_INLINE void assert_##name ();                   \
  BX_SMF BX_CPP_INLINE void clear_##name ();                    \
  BX_SMF BX_CPP_INLINE void set_##name (bx_bool val);

#define IMPLEMENT_EFLAG_ACCESSOR(name,bitnum)                   \
  BX_CPP_INLINE bx_bool BX_CPU_C::getB_##name () {              \
    return 1 & (BX_CPU_THIS_PTR eflags >> bitnum);              \
  }                                                             \
  BX_CPP_INLINE Bit32u  BX_CPU_C::get_##name () {               \
    return BX_CPU_THIS_PTR eflags & (1 << bitnum);              \
  }

#define IMPLEMENT_EFLAG_SET_ACCESSOR(name,bitnum)               \
  BX_CPP_INLINE void BX_CPU_C::assert_##name () {               \
    BX_CPU_THIS_PTR eflags |= (1<<bitnum);                      \
  }                                                             \
  BX_CPP_INLINE void BX_CPU_C::clear_##name () {                \
    BX_CPU_THIS_PTR eflags &= ~(1<<bitnum);                     \
  }                                                             \
  BX_CPP_INLINE void BX_CPU_C::set_##name (bx_bool val) {       \
    BX_CPU_THIS_PTR eflags =                                    \
      (BX_CPU_THIS_PTR eflags&~(1<<bitnum))|((val)<<bitnum);    \
  }

#define EFlagsCFMask   (1 <<  0)
#define EFlagsPFMask   (1 <<  2)
#define EFlagsAFMask   (1 <<  4)
#define EFlagsZFMask   (1 <<  6)
#define EFlagsSFMask   (1 <<  7)
#define EFlagsTFMask   (1 <<  8)
#define EFlagsIFMask   (1 <<  9)
#define EFlagsDFMask   (1 << 10)
#define EFlagsOFMask   (1 << 11)
#define EFlagsIOPLMask (3 << 12)
#define EFlagsNTMask   (1 << 14)
#define EFlagsRFMask   (1 << 16)
#define EFlagsVMMask   (1 << 17)
#define EFlagsACMask   (1 << 18)
#define EFlagsVIFMask  (1 << 19)
#define EFlagsVIPMask  (1 << 20)
#define EFlagsIDMask   (1 << 21)

#define EFlagsOSZAPCMask \
    (EFlagsCFMask | EFlagsPFMask | EFlagsAFMask | EFlagsZFMask | EFlagsSFMask | EFlagsOFMask)

#define EFlagsOSZAPMask  \
    (EFlagsPFMask | EFlagsAFMask | EFlagsZFMask | EFlagsSFMask | EFlagsOFMask)

#define EFlagsValidMask  0x003f7fd5	// only supported bits for EFLAGS

// general purpose register
#ifdef BX_BIG_ENDIAN

typedef struct {
  union {
    struct {
      Bit32u dword_filler;
      Bit16u  word_filler;
      union {
        Bit16u rx;
        struct {
          Bit8u rh;
          Bit8u rl;
        } byte;
      };
    } word;
    Bit64u rrx;
    struct {
      Bit32u hrx;  // hi 32 bits
      Bit32u erx;  // lo 32 bits
    } dword;
  };
} bx_gen_reg_t;
#else
typedef struct {
  union {
    struct {
      union {
        Bit16u rx;
        struct {
          Bit8u rl;
          Bit8u rh;
        } byte;
      };
      Bit16u  word_filler;
      Bit32u dword_filler;
    } word;
    Bit64u rrx;
    struct {
      Bit32u erx;  // lo 32 bits
      Bit32u hrx;  // hi 32 bits
    } dword;
  };
} bx_gen_reg_t;

#endif

#if BX_SUPPORT_FPU
#include "cpu/i387.h"
#include "cpu/xmm.h"
#endif

#include "cpu/instr.h"
#include "cpu/lazy_flags.h"

#define Is64BitMode()    (1)

#endif
