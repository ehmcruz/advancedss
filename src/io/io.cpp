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
#include "../host_code/host.h"

using namespace io;

/****************/
// callback tables

struct io_table_handler {
	void (*callback)(void *data, uint8_t size);
};

static io_table_handler tb_io_read[IO_NUMBER_OF_ADDR];
static io_table_handler tb_io_write[IO_NUMBER_OF_ADDR];

/********************/

// class pointers for when inside static member functions

namespace io {
	video_t *video;
	memory_t *memory;
	keyboard_t *keyboard;
	external_hd_t *external_hd;
	int_controller_t *int_controller;
	timer_t *timer;
};

/*************************************************************/

io::device_t::device_t()
{
	this->int_controller = get_int_controller();
}

void io::advancedss_io_initialize()
{
	uint32_t i;

	memory = new memory_t();

	for (i=0; i<IO_NUMBER_OF_ADDR; i++) {
		tb_io_read[i].callback = NULL;
		tb_io_write[i].callback = NULL;
	}

	int_controller = new int_controller_t; // must be the first device

	video = new video_t;
	keyboard = new keyboard_t;
	external_hd = new external_hd_t;
	timer = new timer_t;
}

void io::cycle()
{
	int_controller->pre_cycle();
	video->cycle();
	keyboard->cycle();
	external_hd->cycle();
	timer->cycle();
	int_controller->pos_cycle();
}

void io::register_io_callback_read(uint32_t pos, void (*callback)(void *data, uint8_t size))
{
	tb_io_read[pos].callback = callback;
}

void io::register_io_callback_write(uint32_t pos, void (*callback)(void *data, uint8_t size))
{
	tb_io_write[pos].callback = callback;
}

void io::call_io_handler_read(uint32_t pos, void *data, uint8_t size)
{
	void (*callback)(void *data, uint8_t size);

	callback = tb_io_read[pos].callback;

	if (callback != NULL) {
		(*callback)(data, size);
	}
}

void io::call_io_handler_write(uint32_t pos, void *data, uint8_t size)
{
	void (*callback)(void *data, uint8_t size);

	callback = tb_io_write[pos].callback;

	if (callback != NULL) {
		(*callback)(data, size);
	}
}

/*************************************************************/

memory_t* io::get_memory_obj()
{
	return memory;
}

int_controller_t* io::get_int_controller()
{
	return int_controller;
}
