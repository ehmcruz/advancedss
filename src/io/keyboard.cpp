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
#include "io.h"
#include "../config_file.h"
#include "../host_code/host.h"

using namespace io;

#define IO_KEYBOARD_HAS_KEY     0x01

/*************************************************************/

io::keyboard_t::keyboard_t(): device_t()
{
	this->state = 0;
	this->must_interrupt = 0;

	register_io_callback_read(IO_PORTS_READ_KEYBOARD_STATUS,
	                           &keyboard_t::io_read_port_status);

	register_io_callback_read(IO_PORTS_READ_KEYBOARD_DATA,
	                           &keyboard_t::io_read_port_data);
}

void io::keyboard_t::io_read_port_status(void *data, uint8_t size)
{
	*((uint8_t*)data) = keyboard->state;
}

void io::keyboard_t::io_read_port_data(void *data, uint8_t size)
{
	*((uint8_t*)data) = keyboard->data;
	keyboard->state &= ~IO_KEYBOARD_HAS_KEY;
	keyboard->must_interrupt = 0;
}

void io::keyboard_t::cycle()
{
	uint16_t r;
	
	if (this->must_interrupt) {
		this->int_controller->request_int(this, IO_INTERRUPT_CODE_KEYBOARD);
		return;
	}

	if ((this->state & IO_KEYBOARD_HAS_KEY) == 0) {
		r = host_sim_input_get_input_if_there_is();

		if (r & HOST_CODE_THERE_IS_INPUT) {
			this->state |= IO_KEYBOARD_HAS_KEY;
			this->data = HOST_CODE_GET_INPUT(r);
			this->int_controller->request_int(this, IO_INTERRUPT_CODE_KEYBOARD);
		}
	}
}

void io::keyboard_t::interrupt_win()
{
	this->must_interrupt = 0;
}

void io::keyboard_t::interrupt_lost()
{
	this->must_interrupt = 1;
}
