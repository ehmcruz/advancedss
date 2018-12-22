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

    January 2009
*/

#ifndef _ADVANCEDSS_X8664_GENERAL_END_HEADER_
#define _ADVANCEDSS_X8664_GENERAL_END_HEADER_

#include "bochs/general_end.h"

inline void processor_t::set_pc(target_addr_t pc) {
	//			if (pc == 0x4022b2) {LOG_PRINTF("pc setting to 0x"PRINTF_INT64X_PREFIX" at rip=0x"PRINTF_INT64X_PREFIX"\n",pc, this->pc);}
	//				if (pc == 0x4022b1) {LOG_PRINTF("pc setting to 0x"PRINTF_INT64X_PREFIX" at rip=0x"PRINTF_INT64X_PREFIX"\n",pc, this->pc);}
	//		if (pc >= 0x400000) {LOG_PRINTF("<pc>|"PRINTF_INT64X_PREFIX"\n",pc);}
	RIP = pc;//LOG_PRINTF("pc set to 0x"PRINTF_INT64X_PREFIX"\n",pc);
}

inline target_addr_t processor_t::get_pc() {
	return RIP;
}

#endif
