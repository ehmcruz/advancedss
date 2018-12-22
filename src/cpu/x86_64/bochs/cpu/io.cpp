/////////////////////////////////////////////////////////////////////////
// $Id: io.cc,v 1.78 2009/04/07 16:12:19 sshwarts Exp $
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

//#include "iodev/iodev.h"

#if BX_SUPPORT_X86_64==0
// Make life easier for merging cpu64 and cpu32 code.
#define RDI EDI
#define RSI ESI
#define RAX EAX
#define RCX ECX
#endif

//
// REP INS methods
//

void BX_CPP_AttrRegparmN(1) BX_CPU_C::REP_INSB_YbDX(bxInstruction_c *i)
{
  if (! allow_io(i, DX, 1)) {
    BX_DEBUG(("INSB_YbDX: I/O access not allowed !"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

#if BX_SUPPORT_X86_64
  if (i->as64L()) {
    BX_CPU_THIS_PTR repeat(i, &BX_CPU_C::INSB64_YbDX);
  }
  else
#endif
  if (i->as32L()) {
    BX_CPU_THIS_PTR repeat(i, &BX_CPU_C::INSB32_YbDX);
    BX_CLEAR_64BIT_HIGH(BX_64BIT_REG_RDI); // always clear upper part of RDI
  }
  else {
    //BX_CPU_THIS_PTR repeat(i, &BX_CPU_C::INSB16_YbDX);
    BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
  }
}

// 16-bit address size
void BX_CPP_AttrRegparmN(1) BX_CPU_C::INSB16_YbDX(bxInstruction_c *i)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
  /*
  // trigger any segment or page faults before reading from IO port
  Bit8u value8 = read_RMW_virtual_byte_32(BX_SEG_REG_ES, DI);

  value8 = BX_INP(DX, 1);

  write_RMW_virtual_byte(value8);

  if (BX_CPU_THIS_PTR get_DF())
    DI--;
  else
    DI++;*/
}

// 32-bit address size
void BX_CPP_AttrRegparmN(1) BX_CPU_C::INSB32_YbDX(bxInstruction_c *i)
{
  // trigger any segment or page faults before reading from IO port
  Bit8u value8 = read_RMW_virtual_byte_32(BX_SEG_REG_ES, EDI);

  value8 = BX_INP(DX, 1);

  /* no seg override possible */
  write_RMW_virtual_byte(value8);

  if (BX_CPU_THIS_PTR get_DF()) {
    RDI = EDI - 1;
  }
  else {
    RDI = EDI + 1;
  }
}

#if BX_SUPPORT_X86_64

// 64-bit address size
void BX_CPP_AttrRegparmN(1) BX_CPU_C::INSB64_YbDX(bxInstruction_c *i)
{
  // trigger any segment or page faults before reading from IO port
  Bit8u value8 = read_RMW_virtual_byte_64(BX_SEG_REG_ES, RDI);

  value8 = BX_INP(DX, 1);

  write_RMW_virtual_byte(value8);

  if (BX_CPU_THIS_PTR get_DF())
    RDI--;
  else
    RDI++;
}

#endif

void BX_CPP_AttrRegparmN(1) BX_CPU_C::REP_INSW_YwDX(bxInstruction_c *i)
{
  if (! allow_io(i, DX, 2)) {
    BX_DEBUG(("INSW_YwDX: I/O access not allowed !"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

#if BX_SUPPORT_X86_64
  if (i->as64L()) {
    BX_CPU_THIS_PTR repeat(i, &BX_CPU_C::INSW64_YwDX);
  }
  else
#endif
  if (i->as32L()) {
    BX_CPU_THIS_PTR repeat(i, &BX_CPU_C::INSW32_YwDX);
    BX_CLEAR_64BIT_HIGH(BX_64BIT_REG_RDI); // always clear upper part of RDI
  }
  else {
    //BX_CPU_THIS_PTR repeat(i, &BX_CPU_C::INSW16_YwDX);
    BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
  }
}

// 16-bit operand size, 16-bit address size
void BX_CPP_AttrRegparmN(1) BX_CPU_C::INSW16_YwDX(bxInstruction_c *i)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
  /*
  // trigger any segment or page faults before reading from IO port
  Bit16u value16 = read_RMW_virtual_word_32(BX_SEG_REG_ES, DI);

  value16 = BX_INP(DX, 2);

  write_RMW_virtual_word(value16);

  if (BX_CPU_THIS_PTR get_DF())
    DI -= 2;
  else
    DI += 2;*/
}

// 16-bit operand size, 32-bit address size
void BX_CPP_AttrRegparmN(1) BX_CPU_C::INSW32_YwDX(bxInstruction_c *i)
{
  Bit16u value16=0;
  Bit32u edi = EDI;
  unsigned incr = 2;

    // trigger any segment or page faults before reading from IO port
    value16 = read_RMW_virtual_word_32(BX_SEG_REG_ES, edi);

    value16 = BX_INP(DX, 2);

    write_RMW_virtual_word(value16);

  if (BX_CPU_THIS_PTR get_DF())
    RDI = EDI - incr;
  else
    RDI = EDI + incr;
}

#if BX_SUPPORT_X86_64

// 16-bit operand size, 64-bit address size
void BX_CPP_AttrRegparmN(1) BX_CPU_C::INSW64_YwDX(bxInstruction_c *i)
{
  // trigger any segment or page faults before reading from IO port
  Bit16u value16 = read_RMW_virtual_word_64(BX_SEG_REG_ES, RDI);

  value16 = BX_INP(DX, 2);

  write_RMW_virtual_word(value16);

  if (BX_CPU_THIS_PTR get_DF())
    RDI -= 2;
  else
    RDI += 2;
}

#endif

void BX_CPP_AttrRegparmN(1) BX_CPU_C::REP_INSD_YdDX(bxInstruction_c *i)
{
  if (! allow_io(i, DX, 4)) {
    BX_DEBUG(("INSD_YdDX: I/O access not allowed !"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

#if BX_SUPPORT_X86_64
  if (i->as64L()) {
    BX_CPU_THIS_PTR repeat(i, &BX_CPU_C::INSD64_YdDX);
  }
  else
#endif
  if (i->as32L()) {
    BX_CPU_THIS_PTR repeat(i, &BX_CPU_C::INSD32_YdDX);
    BX_CLEAR_64BIT_HIGH(BX_64BIT_REG_RDI); // always clear upper part of RDI
  }
  else {
    //BX_CPU_THIS_PTR repeat(i, &BX_CPU_C::INSD16_YdDX);
    BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
  }
}

// 32-bit operand size, 16-bit address size
void BX_CPP_AttrRegparmN(1) BX_CPU_C::INSD16_YdDX(bxInstruction_c *i)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
  /*
  // trigger any segment or page faults before reading from IO port
  Bit32u value32 = read_RMW_virtual_dword_32(BX_SEG_REG_ES, DI);

  value32 = BX_INP(DX, 4);

  write_RMW_virtual_dword(value32);

  if (BX_CPU_THIS_PTR get_DF())
    DI -= 4;
  else
    DI += 4;*/
}

// 32-bit operand size, 32-bit address size
void BX_CPP_AttrRegparmN(1) BX_CPU_C::INSD32_YdDX(bxInstruction_c *i)
{
  // trigger any segment or page faults before reading from IO port
  Bit32u value32 = read_RMW_virtual_dword_32(BX_SEG_REG_ES, EDI);

  value32 = BX_INP(DX, 4);

  write_RMW_virtual_dword(value32);

  if (BX_CPU_THIS_PTR get_DF())
    RDI = EDI - 4;
  else
    RDI = EDI + 4;
}

#if BX_SUPPORT_X86_64

// 32-bit operand size, 64-bit address size
void BX_CPP_AttrRegparmN(1) BX_CPU_C::INSD64_YdDX(bxInstruction_c *i)
{
  // trigger any segment or page faults before reading from IO port
  Bit32u value32 = read_RMW_virtual_dword_64(BX_SEG_REG_ES, RDI);

  value32 = BX_INP(DX, 4);

  write_RMW_virtual_dword(value32);

  if (BX_CPU_THIS_PTR get_DF())
    RDI -= 4;
  else
    RDI += 4;
}

#endif

//
// REP OUTS methods
//

void BX_CPP_AttrRegparmN(1) BX_CPU_C::REP_OUTSB_DXXb(bxInstruction_c *i)
{
  if (! allow_io(i, DX, 1)) {
    BX_DEBUG(("OUTSB_DXXb: I/O access not allowed !"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

#if BX_SUPPORT_X86_64
  if (i->as64L()) {
    BX_CPU_THIS_PTR repeat(i, &BX_CPU_C::OUTSB64_DXXb);
  }
  else
#endif
  if (i->as32L()) {
    BX_CPU_THIS_PTR repeat(i, &BX_CPU_C::OUTSB32_DXXb);
    BX_CLEAR_64BIT_HIGH(BX_64BIT_REG_RSI); // always clear upper part of RSI
  }
  else {
    //BX_CPU_THIS_PTR repeat(i, &BX_CPU_C::OUTSB16_DXXb);
    BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
  }
}

// 16-bit address size
void BX_CPP_AttrRegparmN(1) BX_CPU_C::OUTSB16_DXXb(bxInstruction_c *i)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
  /*Bit8u value8 = read_virtual_byte_32(i->seg(), SI);
  BX_OUTP(DX, value8, 1);

  if (BX_CPU_THIS_PTR get_DF())
    SI--;
  else
    SI++;*/
}

// 32-bit address size
void BX_CPP_AttrRegparmN(1) BX_CPU_C::OUTSB32_DXXb(bxInstruction_c *i)
{
  Bit8u value8 = read_virtual_byte(i->seg(), ESI);
  BX_OUTP(DX, value8, 1);

  if (BX_CPU_THIS_PTR get_DF())
    RSI = ESI - 1;
  else
    RSI = ESI + 1;
}

#if BX_SUPPORT_X86_64

// 64-bit address size
void BX_CPP_AttrRegparmN(1) BX_CPU_C::OUTSB64_DXXb(bxInstruction_c *i)
{
  Bit8u value8 = read_virtual_byte_64(i->seg(), RSI);
  BX_OUTP(DX, value8, 1);

  if (BX_CPU_THIS_PTR get_DF())
    RSI--;
  else
    RSI++;
}

#endif

void BX_CPP_AttrRegparmN(1) BX_CPU_C::REP_OUTSW_DXXw(bxInstruction_c *i)
{
  if (! allow_io(i, DX, 2)) {
    BX_DEBUG(("OUTSW_DXXw: I/O access not allowed !"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

#if BX_SUPPORT_X86_64
  if (i->as64L()) {
    BX_CPU_THIS_PTR repeat(i, &BX_CPU_C::OUTSW64_DXXw);
  }
  else
#endif
  if (i->as32L()) {
    BX_CPU_THIS_PTR repeat(i, &BX_CPU_C::OUTSW32_DXXw);
    BX_CLEAR_64BIT_HIGH(BX_64BIT_REG_RSI); // always clear upper part of RSI
  }
  else {
    //BX_CPU_THIS_PTR repeat(i, &BX_CPU_C::OUTSW16_DXXw);
    BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
  }
}

// 16-bit operand size, 16-bit address size
void BX_CPP_AttrRegparmN(1) BX_CPU_C::OUTSW16_DXXw(bxInstruction_c *i)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
  /*Bit16u value16 = read_virtual_word_32(i->seg(), SI);
  BX_OUTP(DX, value16, 2);

  if (BX_CPU_THIS_PTR get_DF())
    SI -= 2;
  else
    SI += 2;*/
}

// 16-bit operand size, 32-bit address size
void BX_CPP_AttrRegparmN(1) BX_CPU_C::OUTSW32_DXXw(bxInstruction_c *i)
{
  Bit16u value16;
  Bit32u esi = ESI;
  unsigned incr = 2;

    value16 = read_virtual_word(i->seg(), esi);
    BX_OUTP(DX, value16, 2);

  if (BX_CPU_THIS_PTR get_DF())
    RSI = ESI - incr;
  else
    RSI = ESI + incr;
}

#if BX_SUPPORT_X86_64

// 16-bit operand size, 64-bit address size
void BX_CPP_AttrRegparmN(1) BX_CPU_C::OUTSW64_DXXw(bxInstruction_c *i)
{
  Bit16u value16 = read_virtual_word_64(i->seg(), RSI);
  BX_OUTP(DX, value16, 2);

  if (BX_CPU_THIS_PTR get_DF())
    RSI -= 2;
  else
    RSI += 2;
}

#endif

void BX_CPP_AttrRegparmN(1) BX_CPU_C::REP_OUTSD_DXXd(bxInstruction_c *i)
{
  if (! allow_io(i, DX, 4)) {
    BX_DEBUG(("OUTSD_DXXd: I/O access not allowed !"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

#if BX_SUPPORT_X86_64
  if (i->as64L()) {
    BX_CPU_THIS_PTR repeat(i, &BX_CPU_C::OUTSD64_DXXd);
  }
  else
#endif
  if (i->as32L()) {
    BX_CPU_THIS_PTR repeat(i, &BX_CPU_C::OUTSD32_DXXd);
    BX_CLEAR_64BIT_HIGH(BX_64BIT_REG_RSI); // always clear upper part of RSI
  }
  else {
    //BX_CPU_THIS_PTR repeat(i, &BX_CPU_C::OUTSD16_DXXd);
    BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
  }
}

// 32-bit operand size, 16-bit address size
void BX_CPP_AttrRegparmN(1) BX_CPU_C::OUTSD16_DXXd(bxInstruction_c *i)
{
  BOCHS_THIS_CANT_HAPPEN_IN_LONG_MODE
  /*Bit32u value32 = read_virtual_dword_32(i->seg(), SI);
  BX_OUTP(DX, value32, 4);

  if (BX_CPU_THIS_PTR get_DF())
    SI -= 4;
  else
    SI += 4;*/
}

// 32-bit operand size, 32-bit address size
void BX_CPP_AttrRegparmN(1) BX_CPU_C::OUTSD32_DXXd(bxInstruction_c *i)
{
  Bit32u value32 = read_virtual_dword(i->seg(), ESI);
  BX_OUTP(DX, value32, 4);

  if (BX_CPU_THIS_PTR get_DF())
    RSI = ESI - 4;
  else
    RSI = ESI + 4;
}

#if BX_SUPPORT_X86_64

// 32-bit operand size, 64-bit address size
void BX_CPP_AttrRegparmN(1) BX_CPU_C::OUTSD64_DXXd(bxInstruction_c *i)
{
  Bit32u value32 = read_virtual_dword_64(i->seg(), RSI);
  BX_OUTP(DX, value32, 4);

  if (BX_CPU_THIS_PTR get_DF())
    RSI -= 4;
  else
    RSI += 4;
}

#endif

//
// non repeatable IN/OUT methods
//

void BX_CPP_AttrRegparmN(1) BX_CPU_C::IN_ALIb(bxInstruction_c *i)
{
  unsigned port = i->Ib();

  if (! allow_io(i, port, 1)) {
    BX_DEBUG(("IN_ALIb: I/O access not allowed !"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  AL = BX_INP(port, 1);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::IN_AXIb(bxInstruction_c *i)
{
  unsigned port = i->Ib();

  if (! allow_io(i, port, 2)) {
    BX_DEBUG(("IN_AXIb: I/O access not allowed !"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  AX = BX_INP(port, 2);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::IN_EAXIb(bxInstruction_c *i)
{
  unsigned port = i->Ib();

  if (! allow_io(i, port, 4)) {
    BX_DEBUG(("IN_EAXIb: I/O access not allowed !"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  RAX = BX_INP(port, 4);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::OUT_IbAL(bxInstruction_c *i)
{
  unsigned port = i->Ib();

  if (! allow_io(i, port, 1)) {
    BX_DEBUG(("OUT_IbAL: I/O access not allowed !"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  BX_OUTP(port, AL, 1);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::OUT_IbAX(bxInstruction_c *i)
{
  unsigned port = i->Ib();

  if (! allow_io(i, port, 2)) {
    BX_DEBUG(("OUT_IbAX: I/O access not allowed !"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  BX_OUTP(port, AX, 2);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::OUT_IbEAX(bxInstruction_c *i)
{
  unsigned port = i->Ib();

  if (! allow_io(i, port, 4)) {
    BX_DEBUG(("OUT_IbEAX: I/O access not allowed !"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  BX_OUTP(port, EAX, 4);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::IN_ALDX(bxInstruction_c *i)
{
  unsigned port = DX;

  if (! allow_io(i, port, 1)) {
    BX_DEBUG(("IN_ALDX: I/O access not allowed !"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  AL = BX_INP(port, 1);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::IN_AXDX(bxInstruction_c *i)
{
  unsigned port = DX;

  if (! allow_io(i, port, 2)) {
    BX_DEBUG(("IN_AXDX: I/O access not allowed !"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  AX = BX_INP(port, 2);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::IN_EAXDX(bxInstruction_c *i)
{
  unsigned port = DX;

  if (! allow_io(i, port, 4)) {
    BX_DEBUG(("IN_EAXDX: I/O access not allowed !"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  RAX = BX_INP(port, 4);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::OUT_DXAL(bxInstruction_c *i)
{
  unsigned port = DX;

  if (! allow_io(i, port, 1)) {
    BX_DEBUG(("OUT_DXAL: I/O access not allowed !"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  BX_OUTP(port, AL, 1);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::OUT_DXAX(bxInstruction_c *i)
{
  unsigned port = DX;

  if (! allow_io(i, port, 2)) {
    BX_DEBUG(("OUT_DXAX: I/O access not allowed !"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  BX_OUTP(port, AX, 2);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::OUT_DXEAX(bxInstruction_c *i)
{
  unsigned port = DX;

  if (! allow_io(i, port, 4)) {
    BX_DEBUG(("OUT_DXEAX: I/O access not allowed !"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  BX_OUTP(port, EAX, 4);
}
