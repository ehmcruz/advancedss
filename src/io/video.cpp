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

/*************************************************************/

io::video_t::video_t()
{
	this->mem = get_memory_obj();
	this->buffer_addr = 0;

	IO_LIB_DATA_INITIALIZE(this->buffer_addr)
	IO_LIB_DATA_INITIALIZE(this->width)
	IO_LIB_DATA_INITIALIZE(this->height)
	IO_LIB_DATA_INITIALIZE(this->cursor_pos_x)
	IO_LIB_DATA_INITIALIZE(this->cursor_pos_y)

	register_io_callback_write(IO_PORTS_WRITE_VIDEO_SET_BUFFER,
	                           &video_t::io_write_port_set_buffer_addr);

	register_io_callback_write(IO_PORTS_WRITE_VIDEO_CMD,
	                           &video_t::io_write_port_cmd);

	register_io_callback_write(IO_PORTS_WRITE_VIDEO_WIDTH,
	                           &video_t::io_write_port_set_width);

	register_io_callback_write(IO_PORTS_WRITE_VIDEO_HEIGHT,
	                           &video_t::io_write_port_set_height);

	register_io_callback_write(IO_PORTS_WRITE_VIDEO_CURSOR_X,
	                           &video_t::io_write_port_set_cursor_pos_x);

	register_io_callback_write(IO_PORTS_WRITE_VIDEO_CURSOR_Y,
	                           &video_t::io_write_port_set_cursor_pos_y);
}

void io::video_t::io_write_port_set_buffer_addr(void *data, uint8_t size)
{
	IO_LIB_GET_DATA(video->buffer_addr, sizeof(uint32_t), data, size, 1)
}

void io::video_t::io_write_port_set_width(void *data, uint8_t size)
{
	IO_LIB_GET_DATA(video->width, sizeof(uint32_t), data, size, 1)

	host_sim_video_set_width(video->width);
}

void io::video_t::io_write_port_set_height(void *data, uint8_t size)
{
	IO_LIB_GET_DATA(video->height, sizeof(uint32_t), data, size, 1)

	host_sim_video_set_height(video->height);
}

void io::video_t::io_write_port_set_cursor_pos_x(void *data, uint8_t size)
{
	IO_LIB_GET_DATA(video->cursor_pos_x, sizeof(uint32_t), data, size, 1)
}

void io::video_t::io_write_port_set_cursor_pos_y(void *data, uint8_t size)
{
	IO_LIB_GET_DATA(video->cursor_pos_y, sizeof(uint32_t), data, size, 1)
}

void io::video_t::io_write_port_cmd(void *data, uint8_t size)
{
	uint8_t cmd;

	cmd = *((uint8_t*)data);

	switch (cmd) {
		case IO_VIDEO_CMD_REFRESH:
			host_sim_video_refresh((uint8_t*)video->mem->get_vector() + video->buffer_addr);
			break;

		case IO_VIDEO_CMD_INIT:
			host_sim_video_start();
			break;

		case IO_VIDEO_CMD_SET_CURSOR_POS:
			host_sim_video_set_cursor_pos(video->cursor_pos_x, video->cursor_pos_y);
			break;
	}
}

void io::video_t::cycle()
{

}
