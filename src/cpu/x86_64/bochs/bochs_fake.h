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

#ifndef __BOCHS_FAKE_HEADER__H_
#define __BOCHS_FAKE_HEADER__H_

#include "bochs.h"

// used by the fake code added

#define CPU_X8664_IS_IN_KERNEL_MODE \
	(this->cpl == CPU_X8664_CPL_KERNEL)

#define CPU_X8664_CHECK_KERNEL_MODE \
	if (!CPU_X8664_IS_IN_KERNEL_MODE) { \
		exception(BX_GP_EXCEPTION, 0, 0); \
	}

#define CPU_X86_64_ENABLE_INTERRUPTIONS \
	this->enable_interruption();

#define CPU_X86_64_DISABLE_INTERRUPTIONS \
	this->disable_interruption();

#define CPU_X8664_SET_CPL(V) \
	this->set_cpl(V);

#define CPU_X8664_READ_MSR(R) \
	this->read_model_specific_reg(R)

#define CPU_X8664_WRITE_MSR(R, V) \
	this->write_model_specific_reg(R, V);

// decoder stuff

#define BOCHS_CPU_DECODER_INST_MUST_FLUSH_PIPELINE \
	this->bochs_exec_status->must_flush_pipeline = 1;

#define BOCHS_SET_RESULT_TYPE_SYSTEM_HALT           \
	this->bochs_exec_status->is_halt = 1;

#define BOCHS_SET_RESULT_TYPE_BRANCH(TAKEN, TARGET)   \
	this->bochs_exec_status->is_branch = 1;  \
	this->bochs_exec_status->branch_taken = TAKEN;  \
	this->bochs_exec_status->branch_target = TARGET;

#define BOCHS_SET_RESULT_TYPE_TRAP(CODE)       \
	this->bochs_exec_status->is_trap = 1;  \
	this->bochs_exec_status->trap_code = CODE;

/**************************************************************/

// used by the original bochs

#define allow_io(i, port, bytes)       CPU_X8664_IS_IN_KERNEL_MODE

#endif
