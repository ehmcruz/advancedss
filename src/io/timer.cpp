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

#include "../config.h"
#include <pthread.h>
#include "io.h"
#include "../config_file.h"
#include "../main.h"
#include "../host_code/host.h"

using namespace io;

/*************************************************************/

io::timer_t::timer_t(): device_t()
{
	advancedss_config *cfg = get_advancedss_main_config();

	this->counter = 0;
	this->must_interrupt = 0;
	this->n_cpus = advancedss_get_total_number_of_cpus();
	this->max_cycles = cfg->read_int32("timer_max_cycles_to_interrupt") / this->n_cpus;
	this->cpu_to_interrupt = 0;
}

void io::timer_t::cycle()
{
	if (this->counter++ > this->max_cycles) {
		this->must_interrupt = 1;//LOG_PRINTF("requet external int %llu\n",advancedss_get_system_cycle());
		this->int_controller->request_int(this, IO_INTERRUPT_CODE_TIMER, this->cpu_to_interrupt);
	}
}

void io::timer_t::interrupt_win()
{
	this->counter = 0;
	this->cpu_to_interrupt = (this->cpu_to_interrupt + 1) % this->n_cpus;
	this->must_interrupt = 0;//LOG_PRINTF("won external int\n",advancedss_get_system_cycle());
}

void io::timer_t::interrupt_lost()
{
	this->counter = 0;
	this->cpu_to_interrupt = (this->cpu_to_interrupt + 1) % this->n_cpus;
	this->must_interrupt = 0;
}
