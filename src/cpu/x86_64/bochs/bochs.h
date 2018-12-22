/////////////////////////////////////////////////////////////////////////
// $Id: bochs.h,v 1.248 2009/04/26 06:56:27 vruppert Exp $
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
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

//
// bochs.h is the master header file for all C++ code.  It includes all
// the system header files needed by bochs, and also includes all the bochs
// C++ header files.  Because bochs.h and the files that it includes has
// structure and class definitions, it cannot be called from C code.
//

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

#ifndef BX_BOCHS_H
#  define BX_BOCHS_H 1

#include "../../../config.h"

#define BX_PANIC(...) {SANITY_ASSERT(0)}
#define BX_ASSERT(V) {SANITY_ASSERT(V)}

#define BX_SUPPORT_X86_64 1
#define BX_LITTLE_ENDIAN 1
#define BX_C_INLINE static inline
#define BX_CPP_INLINE inline
#define BX_USE_CPU_SMF 0

#define BOCHSAPI
#define BOCHSAPI_MSVCONLY

#define BX_CPP_AttrRegparmN(N)
#define BX_CPU_LEVEL  6

//#define NEED_CPU_REG_SHORTCUTS 1

#define BX_INSTRUMENTATION 0

#include "instrument.h"

#define BX_DEBUGGER 0
#define BX_DEBUG(...)
#define BX_INFO(...)
#define BX_ERROR(...)

typedef uint8_t Bit8u;
typedef int8_t Bit8s;
typedef uint16_t Bit16u;
typedef int16_t Bit16s;
typedef uint32_t Bit32u;
typedef int32_t Bit32s;
typedef uint64_t Bit64u;
typedef int64_t Bit64s;

typedef Bit64u bx_address;

typedef Bit32u bx_bool;

#ifdef BX_LITTLE_ENDIAN

#define WriteHostWordToLittleEndian(hostPtr,  nativeVar16) \
    *((Bit16u*)(hostPtr)) = (nativeVar16)
#define WriteHostDWordToLittleEndian(hostPtr, nativeVar32) \
    *((Bit32u*)(hostPtr)) = (nativeVar32)
#define WriteHostQWordToLittleEndian(hostPtr, nativeVar64) \
    *((Bit64u*)(hostPtr)) = (nativeVar64)

#define ReadHostWordFromLittleEndian(hostPtr,  nativeVar16) \
    (nativeVar16) = *((Bit16u*)(hostPtr))
#define ReadHostDWordFromLittleEndian(hostPtr, nativeVar32) \
    (nativeVar32) = *((Bit32u*)(hostPtr))
#define ReadHostQWordFromLittleEndian(hostPtr, nativeVar64) \
    (nativeVar64) = *((Bit64u*)(hostPtr))

#define CopyHostWordLittleEndian(hostAddrDst,  hostAddrSrc)  \
    (* (Bit16u *)(hostAddrDst)) = (* (Bit16u *)(hostAddrSrc));
#define CopyHostDWordLittleEndian(hostAddrDst,  hostAddrSrc) \
    (* (Bit32u *)(hostAddrDst)) = (* (Bit32u *)(hostAddrSrc));
#define CopyHostQWordLittleEndian(hostAddrDst,  hostAddrSrc) \
    (* (Bit64u *)(hostAddrDst)) = (* (Bit64u *)(hostAddrSrc));

#else

#define WriteHostWordToLittleEndian(hostPtr,  nativeVar16) { \
    ((Bit8u *)(hostPtr))[0] = (Bit8u)  (nativeVar16);        \
    ((Bit8u *)(hostPtr))[1] = (Bit8u) ((nativeVar16)>>8);    \
}
#define WriteHostDWordToLittleEndian(hostPtr, nativeVar32) { \
    ((Bit8u *)(hostPtr))[0] = (Bit8u)  (nativeVar32);        \
    ((Bit8u *)(hostPtr))[1] = (Bit8u) ((nativeVar32)>>8);    \
    ((Bit8u *)(hostPtr))[2] = (Bit8u) ((nativeVar32)>>16);   \
    ((Bit8u *)(hostPtr))[3] = (Bit8u) ((nativeVar32)>>24);   \
}
#define WriteHostQWordToLittleEndian(hostPtr, nativeVar64) { \
    ((Bit8u *)(hostPtr))[0] = (Bit8u)  (nativeVar64);        \
    ((Bit8u *)(hostPtr))[1] = (Bit8u) ((nativeVar64)>>8);    \
    ((Bit8u *)(hostPtr))[2] = (Bit8u) ((nativeVar64)>>16);   \
    ((Bit8u *)(hostPtr))[3] = (Bit8u) ((nativeVar64)>>24);   \
    ((Bit8u *)(hostPtr))[4] = (Bit8u) ((nativeVar64)>>32);   \
    ((Bit8u *)(hostPtr))[5] = (Bit8u) ((nativeVar64)>>40);   \
    ((Bit8u *)(hostPtr))[6] = (Bit8u) ((nativeVar64)>>48);   \
    ((Bit8u *)(hostPtr))[7] = (Bit8u) ((nativeVar64)>>56);   \
}

#define ReadHostWordFromLittleEndian(hostPtr, nativeVar16) {   \
    (nativeVar16) =  ((Bit16u) ((Bit8u *)(hostPtr))[0]) |      \
                    (((Bit16u) ((Bit8u *)(hostPtr))[1])<<8) ;  \
}
#define ReadHostDWordFromLittleEndian(hostPtr, nativeVar32) {  \
    (nativeVar32) =  ((Bit32u) ((Bit8u *)(hostPtr))[0]) |      \
                    (((Bit32u) ((Bit8u *)(hostPtr))[1])<<8) |  \
                    (((Bit32u) ((Bit8u *)(hostPtr))[2])<<16) | \
                    (((Bit32u) ((Bit8u *)(hostPtr))[3])<<24);  \
}
#define ReadHostQWordFromLittleEndian(hostPtr, nativeVar64) {  \
    (nativeVar64) =  ((Bit64u) ((Bit8u *)(hostPtr))[0]) |      \
                    (((Bit64u) ((Bit8u *)(hostPtr))[1])<<8) |  \
                    (((Bit64u) ((Bit8u *)(hostPtr))[2])<<16) | \
                    (((Bit64u) ((Bit8u *)(hostPtr))[3])<<24) | \
                    (((Bit64u) ((Bit8u *)(hostPtr))[4])<<32) | \
                    (((Bit64u) ((Bit8u *)(hostPtr))[5])<<40) | \
                    (((Bit64u) ((Bit8u *)(hostPtr))[6])<<48) | \
                    (((Bit64u) ((Bit8u *)(hostPtr))[7])<<56);  \
}

#define CopyHostWordLittleEndian(hostAddrDst, hostAddrSrc) {   \
    ((Bit8u *)(hostAddrDst))[0] = ((Bit8u *)(hostAddrSrc))[0]; \
    ((Bit8u *)(hostAddrDst))[1] = ((Bit8u *)(hostAddrSrc))[1]; \
}
#define CopyHostDWordLittleEndian(hostAddrDst, hostAddrSrc) {  \
    ((Bit8u *)(hostAddrDst))[0] = ((Bit8u *)(hostAddrSrc))[0]; \
    ((Bit8u *)(hostAddrDst))[1] = ((Bit8u *)(hostAddrSrc))[1]; \
    ((Bit8u *)(hostAddrDst))[2] = ((Bit8u *)(hostAddrSrc))[2]; \
    ((Bit8u *)(hostAddrDst))[3] = ((Bit8u *)(hostAddrSrc))[3]; \
}
#define CopyHostQWordLittleEndian(hostAddrDst, hostAddrSrc) {  \
    ((Bit8u *)(hostAddrDst))[0] = ((Bit8u *)(hostAddrSrc))[0]; \
    ((Bit8u *)(hostAddrDst))[1] = ((Bit8u *)(hostAddrSrc))[1]; \
    ((Bit8u *)(hostAddrDst))[2] = ((Bit8u *)(hostAddrSrc))[2]; \
    ((Bit8u *)(hostAddrDst))[3] = ((Bit8u *)(hostAddrSrc))[3]; \
    ((Bit8u *)(hostAddrDst))[4] = ((Bit8u *)(hostAddrSrc))[4]; \
    ((Bit8u *)(hostAddrDst))[5] = ((Bit8u *)(hostAddrSrc))[5]; \
    ((Bit8u *)(hostAddrDst))[6] = ((Bit8u *)(hostAddrSrc))[6]; \
    ((Bit8u *)(hostAddrDst))[7] = ((Bit8u *)(hostAddrSrc))[7]; \
}

#endif

#if BX_SUPPORT_X86_64
#ifdef BX_LITTLE_ENDIAN
typedef
  struct {
         Bit64u lo;
         Bit64u hi;
         } Bit128u;
typedef
  struct {
         Bit64u lo;
         Bit64s hi;
         } Bit128s;
#else   // must be Big Endian
typedef
  struct {
         Bit64u hi;
         Bit64u lo;
         } Bit128u;
typedef
  struct {
         Bit64s hi;
         Bit64u lo;
         } Bit128s;
#endif
#endif  // #if BX_SUPPORT_X86_64

#define BX_CPU_C cpu::processor_t

#define GET32L(val64) ((Bit32u)(((Bit64u)(val64)) & 0xFFFFFFFF))
#define GET32H(val64) ((Bit32u)(((Bit64u)(val64)) >> 32))

#define BX_CONST64(x)  (x##LL)

// technically, in an 8 bit signed the real minimum is -128, not -127.
// But if you decide to negate -128 you tend to get -128 again, so it's
// better not to use the absolute maximum in the signed range.
#define BX_MAX_BIT64U ( (Bit64u) -1           )
#define BX_MIN_BIT64U ( 0                     )
#define BX_MAX_BIT64S ( ((Bit64u) -1) >> 1    )
#define BX_MIN_BIT64S ( (Bit64s)-(((Bit64u) -1) >> 1) )
#define BX_MAX_BIT32U ( (Bit32u) -1           )
#define BX_MIN_BIT32U ( 0                     )
#define BX_MAX_BIT32S ( ((Bit32u) -1) >> 1    )
#define BX_MIN_BIT32S ( (Bit32s)-(((Bit32u) -1) >> 1) )
#define BX_MAX_BIT16U ( (Bit16u) -1           )
#define BX_MIN_BIT16U ( 0                     )
#define BX_MAX_BIT16S ( ((Bit16u) -1) >> 1    )
#define BX_MIN_BIT16S ( (Bit16s)-(((Bit16u) -1) >> 1) )
#define BX_MAX_BIT8U  ( (Bit8u) -1            )
#define BX_MIN_BIT8U  ( 0                     )
#define BX_MAX_BIT8S  ( ((Bit8u) -1) >> 1     )
#define BX_MIN_BIT8S  ( (Bit8s)-(((Bit8u) -1) >> 1)  )

#define BX_SUPPORT_ALIGNMENT_CHECK 0
#define BX_SUPPORT_FPU 1
#define BX_SUPPORT_MMX 1
#define BX_SUPPORT_3DNOW 0
#define BX_SUPPORT_SSE 2
#define BX_SUPPORT_SSE_EXTENSION 0
#define BX_SUPPORT_SSE4A 0
#define BX_SUPPORT_SSE5A 0
#define BX_SUPPORT_DAZ 0
#define BX_SUPPORT_MISALIGNED_SSE 0
#define BX_SUPPORT_SEP 0
#define BX_SUPPORT_VME 0
#define BX_SUPPORT_POPCNT 0
#define BX_SUPPORT_MONITOR_MWAIT 0
#define BX_SUPPORT_XSAVE 0
#define BX_SUPPORT_AES 0
#define BX_SUPPORT_MOVBE 0
#define BX_SUPPORT_VMX 0

#endif  /* BX_BOCHS_H */
