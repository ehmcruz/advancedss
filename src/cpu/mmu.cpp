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

#include <string.h>
#include "cpu.h"

using namespace cpu;

#define MUTEX_LOCK_IF_NEED          if (this->number_processors_shared > 1) { pthread_mutex_lock(&this->mutex); }
#define MUTEX_UNLOCK_IF_NEED        if (this->number_processors_shared > 1) { pthread_mutex_unlock(&this->mutex); }

cpu::mmu_t::mmu_t(uint32_t number_processors_shared)
{
	this->number_processors_shared = number_processors_shared;
	if (number_processors_shared > 1) {
		pthread_mutex_init(&this->mutex, NULL);
	}
}

