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
#include "../cpu/cpu.h"

using namespace io;

/*************************************************************/

io::int_controller_t::int_controller_t()
{
	this->n_cpus = advancedss_get_total_number_of_cpus();
}

void io::int_controller_t::pre_cycle()
{
	this->n_int_devices = 0;
}

void io::int_controller_t::pos_cycle()
{
	uint8_t i;
	uint32_t j, winner;
	cpu::processor_t *c;

	winner = 0xFFFFFFFF;
	for (i=0; i<this->n_int_devices && winner == 0xFFFFFFFF; i++) {
		c = this->procs[i];
		if (c != NULL) {
			// we never memorize timer interruptions
			if (c->set_external_interrupt(this->codes[i])) {
				this->devices[i]->interrupt_win();
				winner = i;
			}
		}
		else {
			for (j=0; j<this->n_cpus && winner == 0xFFFFFFFF; j++) {
				c = advancedss_get_processor_obj(j);
				if (c->set_external_interrupt(this->codes[i])) {
					this->devices[i]->interrupt_win();
					winner = i;
				}
			}
		}
	}

	if (winner != 0xFFFFFFFF) {
		for (i=0; i<winner; i++) {
			this->devices[i]->interrupt_lost();
		}
		for (i=winner+1; i<this->n_int_devices; i++) {
			this->devices[i]->interrupt_lost();
		}
	}
	else {
		for (i=0; i<this->n_int_devices; i++) {
			this->devices[i]->interrupt_lost();
		}
	}
}

void io::int_controller_t::request_int(device_t *d, uint8_t code)
{
	this->devices[this->n_int_devices] = d;
	this->procs[this->n_int_devices] = NULL;
	this->codes[this->n_int_devices] = code;
	this->n_int_devices++;
}

void io::int_controller_t::request_int(device_t *d, uint8_t code, uint32_t cpuid)
{
	this->devices[this->n_int_devices] = d;
	this->procs[this->n_int_devices] = advancedss_get_processor_obj(cpuid);
	this->codes[this->n_int_devices] = code;

	SANITY_ASSERT( this->procs[this->n_int_devices] != NULL )

	this->n_int_devices++;
}
