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

#define IO_EXTERNAL_HD_STATE_IDLE            0
#define IO_EXTERNAL_HD_STATE_WAIT_FNAME      1
#define IO_EXTERNAL_HD_STATE_WAIT_SEEK       2
#define IO_EXTERNAL_HD_STATE_READING_EXT_HD  3
#define IO_EXTERNAL_HD_STATE_WRITTING_EXT_HD 4
#define IO_EXTERNAL_HD_STATE_WAIT_READ       5
#define IO_EXTERNAL_HD_STATE_WAIT_WBUFFER    9

/*  start/stop set the file name
	after IO_EXTERNAL_HD_CMD_FNAME_START,
	the state changes to IO_EXTERNAL_HD_STATE_WAIT_FNAME
	write to the data port the string of the file name
	the string terminator char does not need to be sent, since the
	simulator will stop to write the fil name only after the cmd
	IO_EXTERNAL_HD_CMD_FNAME_END
	when this cmd arrives, state changes to
	IO_EXTERNAL_HD_STATE_IDLE
*/
#define IO_EXTERNAL_HD_CMD_FNAME_START       0
#define IO_EXTERNAL_HD_CMD_FNAME_END         1

/* Opens the file and save the handler in file_handler
*/
#define IO_EXTERNAL_HD_CMD_OPEN              2

/* Closes the file specified by file_handler
*/
#define IO_EXTERNAL_HD_CMD_CLOSE             3

/*  try to read file_bytes bytes from file_handler
	remember that file_bytes can't be greater than
	IO_EXTERNAL_HD_SECTOR_SIZE
	while reading operation is in process, state changes to
	IO_EXTERNAL_HD_STATE_READING_EXT_HD
	when reading finishes, it saves the data in file_buffer
	save in file_bytes the actual number of bytes read
	the interrupt IO_INTERRUPT_CODE_EXTERNAL_HD occurs and
	the state goes to IO_EXTERNAL_HD_STATE_WAIT_READ
	the user now has to read file_bytes from the
	IO_PORTS_WRITE_EXTERNAL_HD_DATA port, so then
	the state goes back to IO_EXTERNAL_HD_STATE_IDLE
*/
#define IO_EXTERNAL_HD_CMD_READ_SECTOR       4

/*  try to write file_bytes bytes to file_handler
	the data source is file_buffer
	remember that file_bytes can't be greater than
	IO_EXTERNAL_HD_SECTOR_SIZE
	while writting operation is in process, state changes to
	IO_EXTERNAL_HD_STATE_WRITING_EXT_HD
	when writting finishes
	the interrupt IO_INTERRUPT_CODE_EXTERNAL_HD occurs and
	the state goes back to IO_EXTERNAL_HD_STATE_IDLE
*/
#define IO_EXTERNAL_HD_CMD_WRITE_SECTOR      5

/*  try to set the current location in file_handler
	after the cmd is received, state changes to
	IO_EXTERNAL_HD_STATE_WAIT_SEEK
	when the port IO_PORTS_READ_EXTERNAL_HD_DATA
	receives the data, the fseek is performed and
	the state goes back to IO_EXTERNAL_HD_STATE_IDLE
*/
#define IO_EXTERNAL_HD_CMD_SEEK              6

/*  get filesize of file_handler
	saves it to file_bytes
    IO_EXTERNAL_HD_STATE_IDLE
*/
#define IO_EXTERNAL_HD_CMD_GET_FILESIZE      9

/*  writes to file_buffer
	the state changes to IO_EXTERNAL_HD_STATE_WAIT_WBUFFER
    after file_bytes is written to
    IO_PORTS_WRITE_EXTERNAL_HD_DATA, state goes back to
    IO_EXTERNAL_HD_STATE_IDLE
*/
#define IO_EXTERNAL_HD_CMD_WRITE_FILE_BUFFER      10

/*  get position of file_handler
	the state changes to IO_EXTERNAL_HD_STATE_WAIT_READPOS
    after the data is read from
    IO_PORTS_READ_EXTERNAL_HD_DATA, state goes back to
    IO_EXTERNAL_HD_STATE_IDLE
*/
#define IO_EXTERNAL_HD_CMD_GET_FILEPOS      13

#define STATE_GET_STATE(V)   (V & 0x3F)

io::external_hd_t::external_hd_t(): device_t()
{
	advancedss_config *cfg = get_advancedss_main_config();

	this->state = IO_EXTERNAL_HD_STATE_IDLE;
	this->cfg_delay_cycles_read_per_byte =
		cfg->read_int32("external_hd_delay_cycles_read_per_byte");
	this->cfg_delay_cycles_write_per_byte =
		cfg->read_int32("external_hd_delay_cycles_write_per_byte");

	register_io_callback_write(IO_PORTS_WRITE_EXTERNAL_HD_CMD,
	                           &external_hd_t::io_write_port_cmd);

	register_io_callback_write(IO_PORTS_WRITE_EXTERNAL_HD_DATA,
	                           &external_hd_t::io_write_port_data);

	register_io_callback_write(IO_PORTS_WRITE_EXTERNAL_HD_FBYTES,
	                           &external_hd_t::io_write_port_fbytes);

	register_io_callback_write(IO_PORTS_WRITE_EXTERNAL_HD_FHANDLER,
	                           &external_hd_t::io_write_port_fhandler);

	register_io_callback_read(IO_PORTS_READ_EXTERNAL_HD_STATUS,
	                           &external_hd_t::io_read_port_status);

	register_io_callback_read(IO_PORTS_READ_EXTERNAL_HD_DATA,
	                           &external_hd_t::io_read_port_data);

	register_io_callback_read(IO_PORTS_READ_EXTERNAL_HD_FBYTES,
	                           &external_hd_t::io_read_port_fbytes);

	register_io_callback_read(IO_PORTS_READ_EXTERNAL_HD_FHANDLER,
	                           &external_hd_t::io_read_port_fhandler);

	IO_LIB_DATA_INITIALIZE(this->file_buffer)
	IO_LIB_DATA_INITIALIZE(this->fhandler)
	IO_LIB_DATA_INITIALIZE(this->file_bytes)

	this->must_interrupt = 0;

	SANITY_ASSERT(sizeof(FILE*) <= sizeof(uint64_t));
}

void io::external_hd_t::io_read_port_status(void *data, uint8_t size)
{
	*((uint8_t*)data) = external_hd->state;
}

void io::external_hd_t::io_read_port_fhandler(void *data, uint8_t size)
{
	IO_LIB_SET_DATA(data, size, external_hd->fhandler, sizeof(uint64_t), 1)
}

void io::external_hd_t::io_read_port_fbytes(void *data, uint8_t size)
{
	IO_LIB_SET_DATA(data, size, external_hd->file_bytes, sizeof(uint32_t), 1)
}

void io::external_hd_t::io_read_port_data(void *data, uint8_t size)
{
	switch (STATE_GET_STATE(external_hd->state)) {
		case IO_EXTERNAL_HD_STATE_WAIT_READ: {
			IO_LIB_SET_DATA(data, size, external_hd->file_buffer, external_hd->file_bytes, 1)
			external_hd->state = IO_EXTERNAL_HD_STATE_IDLE;
		}
		break;
	}
}

/*************************************/

void io::external_hd_t::io_write_port_fhandler(void *data, uint8_t size)
{
	IO_LIB_GET_DATA(external_hd->fhandler, sizeof(uint64_t), data, size, 1)
	external_hd->file_handler = (FILE*)external_hd->fhandler;
}

void io::external_hd_t::io_write_port_fbytes(void *data, uint8_t size)
{
	IO_LIB_GET_DATA(external_hd->file_bytes, sizeof(uint32_t), data, size, 1)
}

void io::external_hd_t::io_write_port_data(void *data, uint8_t size)
{
	switch (STATE_GET_STATE(external_hd->state)) {
		case IO_EXTERNAL_HD_STATE_WAIT_FNAME: {
			char b[2];
			b[0] = *((char*)data);
			b[1] = 0;
			strcat(external_hd->file_name, b);
		}
		break;

		case IO_EXTERNAL_HD_STATE_WAIT_WBUFFER: {
			IO_LIB_GET_DATA(external_hd->file_buffer, external_hd->file_bytes, data, size, 1)
			external_hd->state = IO_EXTERNAL_HD_STATE_IDLE;
		}
		break;

		case IO_EXTERNAL_HD_STATE_WAIT_SEEK: {
			uint32_t pos, pos_count;
			IO_LIB_GET_DATA(pos, sizeof(uint32_t), data, size, 0)
			fseek(external_hd->file_handler, pos, SEEK_SET);
			external_hd->state = IO_EXTERNAL_HD_STATE_IDLE;
		}
		break;
	}
}

void io::external_hd_t::io_write_port_cmd(void *data, uint8_t size)
{
	uint8_t cmd = *(char*)data;
	switch (STATE_GET_STATE(external_hd->state)) {
		case IO_EXTERNAL_HD_STATE_IDLE:
			switch (cmd) {
				case IO_EXTERNAL_HD_CMD_FNAME_START:
					external_hd->file_name[0] = 0;
					external_hd->state = IO_EXTERNAL_HD_STATE_WAIT_FNAME;
				break;

				case IO_EXTERNAL_HD_CMD_OPEN:
					external_hd->file_handler = fopen(external_hd->file_name, "rb+");
					external_hd->fhandler = (uint64_t)(external_hd->file_handler);
				break;

				case IO_EXTERNAL_HD_CMD_CLOSE:
					fclose(external_hd->file_handler);
				break;

				case IO_EXTERNAL_HD_CMD_READ_SECTOR:
					external_hd->delay_cycles = external_hd->calculate_delay_cycle_read();
					external_hd->state = IO_EXTERNAL_HD_STATE_READING_EXT_HD;
				break;

				case IO_EXTERNAL_HD_CMD_WRITE_SECTOR:
					external_hd->delay_cycles = external_hd->calculate_delay_cycle_write();
					external_hd->state = IO_EXTERNAL_HD_STATE_WRITTING_EXT_HD;
				break;

				case IO_EXTERNAL_HD_CMD_WRITE_FILE_BUFFER:
					external_hd->state = IO_EXTERNAL_HD_STATE_WAIT_WBUFFER;
				break;

				case IO_EXTERNAL_HD_CMD_GET_FILESIZE: {
					uint32_t pos;
					pos = ftell(external_hd->file_handler);
					fseek(external_hd->file_handler, 0, SEEK_END);
					external_hd->file_bytes = ftell(external_hd->file_handler);
					fseek(external_hd->file_handler, pos, SEEK_SET);
				}
				break;

				case IO_EXTERNAL_HD_CMD_GET_FILEPOS: {
					external_hd->file_bytes = ftell(external_hd->file_handler);
				}
				break;

				case IO_EXTERNAL_HD_CMD_SEEK:
					external_hd->state = IO_EXTERNAL_HD_STATE_WAIT_SEEK;
				break;
			}
		break;

		case IO_EXTERNAL_HD_STATE_WAIT_FNAME:
			switch (cmd) {
				case IO_EXTERNAL_HD_CMD_FNAME_END:
					external_hd->state = IO_EXTERNAL_HD_STATE_IDLE;
				break;
			}
		break;
	}
}

void io::external_hd_t::cycle()
{
	if (this->must_interrupt == 1) {
		this->int_controller->request_int(this, IO_INTERRUPT_CODE_EXTERNAL_HD);
		return;
	}

	switch (STATE_GET_STATE(this->state)) {
		case IO_EXTERNAL_HD_STATE_READING_EXT_HD:
			if (this->delay_cycles-- == 0) {
				this->file_bytes = fread(this->file_buffer, 1, this->file_bytes, this->file_handler);
				this->state = (this->file_bytes > 0) ? IO_EXTERNAL_HD_STATE_WAIT_READ : IO_EXTERNAL_HD_STATE_IDLE;
				this->must_interrupt = 1;
				this->int_controller->request_int(this, IO_INTERRUPT_CODE_EXTERNAL_HD);
			}
		break;

		case IO_EXTERNAL_HD_STATE_WRITTING_EXT_HD:
			if (this->delay_cycles-- == 0) {
				this->file_bytes = fwrite(this->file_buffer, 1, this->file_bytes, this->file_handler);
				this->state = IO_EXTERNAL_HD_STATE_IDLE;
				this->must_interrupt = 1;
				this->int_controller->request_int(this, IO_INTERRUPT_CODE_EXTERNAL_HD);
			}
		break;
	}
}

void io::external_hd_t::interrupt_win()
{
	this->must_interrupt = 0;
}

void io::external_hd_t::interrupt_lost()
{
}
