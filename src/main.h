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

#ifndef _ADVANCEDSS_MAIN_HEADER_
#define _ADVANCEDSS_MAIN_HEADER_

#include "config.h"

void advancedss_stop_simulation();
uint8_t advancedss_sim_running();
uint32_t advancedss_get_number_phy_cpu();
uint32_t advancedss_get_number_virtual_cpu_per_phy_cpu();
uint32_t advancedss_get_boot_size();
uint32_t advancedss_get_total_number_of_cpus();

#ifdef CPU_CHECK_EXECUTION_TRACE
	int advancedss_check_correct_pc(uint64_t vaddr);
#endif

#endif
