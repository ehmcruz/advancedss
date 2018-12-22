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

#ifndef _ADVANCEDSS_IO_HEADER_
#define _ADVANCEDSS_IO_HEADER_

#include "../config.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

namespace cpu {
	class processor_t;
};

#define IO_NUMBER_OF_ADDR  0x00010000

#define IO_PORTS_WRITE_VIDEO_SET_BUFFER      0x1000
#define IO_PORTS_WRITE_VIDEO_CMD             0x1001
#define IO_PORTS_WRITE_VIDEO_WIDTH           0x1002
#define IO_PORTS_WRITE_VIDEO_HEIGHT          0x1003
#define IO_PORTS_WRITE_VIDEO_CURSOR_X        0x1004
#define IO_PORTS_WRITE_VIDEO_CURSOR_Y        0x1005

#define IO_PORTS_READ_EXTERNAL_HD_STATUS     0x2000
#define IO_PORTS_READ_EXTERNAL_HD_DATA       0x2001
#define IO_PORTS_READ_EXTERNAL_HD_FBYTES     0x2002
#define IO_PORTS_READ_EXTERNAL_HD_FHANDLER   0x2003

#define IO_PORTS_WRITE_EXTERNAL_HD_CMD       0x2000
#define IO_PORTS_WRITE_EXTERNAL_HD_DATA      0x2001
#define IO_PORTS_WRITE_EXTERNAL_HD_FBYTES    0x2002
#define IO_PORTS_WRITE_EXTERNAL_HD_FHANDLER  0x2003

#define IO_PORTS_READ_KEYBOARD_STATUS        0x3000
#define IO_PORTS_READ_KEYBOARD_DATA          0x3001

#define IO_NUMBER_DEVICES_THAT_INTERRUPT     3

/***************/

#define IO_EXTERNAL_HD_SECTOR_SIZE           20480

#define IO_INTERRUPT_CODE_TIMER              0
#define IO_INTERRUPT_CODE_EXTERNAL_HD        1
#define IO_INTERRUPT_CODE_KEYBOARD           2

#define IO_VIDEO_CMD_REFRESH          0x00
#define IO_VIDEO_CMD_INIT             0x01
#define IO_VIDEO_CMD_SET_CURSOR_POS   0x03

#define IO_LIB_COUNT_VAR(NAME)  NAME##_count

#define IO_LIB_DATA_INITIALIZE(NAME) \
	IO_LIB_COUNT_VAR(NAME) = 0;

#define IO_LIB_GET_DATA(DEST, DEST_SIZE, SRC, SRC_SIZE, DIE)  { \
		uint32_t count = IO_LIB_COUNT_VAR(DEST); \
		uint8_t *ptr_dest = (uint8_t*)&DEST; \
		uint8_t *ptr_src = (uint8_t*)SRC; \
		uint8_t i = 0; \
		\
		ptr_dest += count; \
		\
		while (i++ < SRC_SIZE && count++ < DEST_SIZE) { \
			*ptr_dest = *ptr_src; \
			ptr_dest++; \
			ptr_src++; \
		} \
		IO_LIB_COUNT_VAR(DEST) = count; \
		if (count < DEST_SIZE && (DIE)) { \
			return; \
		} \
		IO_LIB_COUNT_VAR(DEST) = 0; \
	}

#define IO_LIB_SET_DATA(DEST, DEST_SIZE, SRC, SRC_SIZE, DIE)  { \
		uint32_t count = IO_LIB_COUNT_VAR(SRC); \
		uint8_t *ptr_src = (uint8_t*)&SRC; \
		uint8_t *ptr_dest = (uint8_t*)DEST; \
		uint8_t i = 0; \
		\
		ptr_src += count; \
		\
		while (i++ < DEST_SIZE && count++ < SRC_SIZE) { \
			*ptr_dest = *ptr_src; \
			ptr_dest++; \
			ptr_src++; \
		} \
		IO_LIB_COUNT_VAR(SRC) = count; \
		if (count < SRC_SIZE && (DIE)) { \
			return; \
		} \
		IO_LIB_COUNT_VAR(SRC) = 0; \
	}

namespace io {
	class int_controller_t;

	class device_t /* needed by devices that generates interruption */
	{
		protected:
			int_controller_t *int_controller;

		public:
			device_t();

			virtual void interrupt_win() = 0;
			virtual void interrupt_lost() = 0;
	};

	class memory_t
	{
		protected:
			volatile uint8_t *vector;
			uint32_t size;
			pthread_mutex_t mutex;
			volatile uint32_t memory_is_locked;
			volatile uint32_t memory_lockfree_operations_running;
			volatile uint32_t lockfree_operations_pending;
			volatile uint32_t lock_operations_pending;
			pthread_cond_t cond_memory_lock;
			pthread_cond_t cond_memory_lockfree;

		public:
			memory_t();

			void lock();
			void unlock();
			void check_lock();
			void check_unlock();

			inline volatile void* get_vector() {
				return this->vector;
			}

			inline void read(void *dest, uint32_t n, uint32_t addr) {
				memcpy(dest, (const void *)(this->vector + addr), n);
			}

			inline uint8_t read1(uint32_t addr) {
				#ifdef SANITY_CHECK
					if (addr >= this->size) {
						LOG_PRINTF("memory read 1 byte addr = 0x%X\n", addr);
						SANITY_ASSERT(0);
					}
				#endif
				return this->vector[addr];
			}

			inline void write1(uint32_t addr, uint8_t data) {
				#ifdef SANITY_CHECK
					if (addr >= this->size) {
						LOG_PRINTF("memory write 1 byte addr = 0x%X\n", addr);
						SANITY_ASSERT(0);
					}
				#endif
				this->vector[addr] = data;
			}

			inline uint16_t read2(uint32_t addr) {
				volatile uint8_t *pos = this->vector + addr;
				uint16_t data;

				#ifdef SANITY_CHECK
					if ((addr+2) >= this->size) {
						LOG_PRINTF("memory read 2 bytes addr = 0x%X\n", addr);
						SANITY_ASSERT(0);
					}
				#endif

				#ifdef CONFIG_HOST_SYSTEM_LITTLE_ENDIAN
					#ifdef CONFIG_TARGET_SYSTEM_LITTLE_ENDIAN
						data = * ((uint16_t*)pos);
					#else
						jskkls
					#endif
				#else
					jjjsjsdj
				#endif
				return data;
			}

			inline void write2(uint32_t addr, uint16_t data) {
				volatile uint8_t *pos = this->vector + addr;

				#ifdef SANITY_CHECK
					if ((addr+2) >= this->size) {
						LOG_PRINTF("memory write 2 bytes addr = 0x%X\n", addr);
						SANITY_ASSERT(0);
					}
				#endif

				#ifdef CONFIG_HOST_SYSTEM_LITTLE_ENDIAN
					#ifdef CONFIG_TARGET_SYSTEM_LITTLE_ENDIAN
						*((uint16_t*)pos) = data;
					#else
						jjjsjsdj
					#endif
				#else
					jjjsjsdj
				#endif
			}

			inline uint32_t read4(uint32_t addr) {
				volatile uint8_t *pos = this->vector + addr;
				uint32_t data;

				#ifdef SANITY_CHECK
					if ((addr+4) >= this->size) {
						LOG_PRINTF("memory read 4 bytes addr = 0x%X\n", addr);
						SANITY_ASSERT(0);
					}
				#endif

				#ifdef CONFIG_HOST_SYSTEM_LITTLE_ENDIAN
					#ifdef CONFIG_TARGET_SYSTEM_LITTLE_ENDIAN
						data = * ((uint32_t*)pos);
					#else
						jjjsjsdj
					#endif
				#else
					jjjsjsdj
				#endif
				return data;
			}

			inline void write4(uint32_t addr, uint32_t data) {
				volatile uint8_t *pos = this->vector + addr;

				#ifdef SANITY_CHECK
					if ((addr+4) >= this->size) {
						LOG_PRINTF("memory write 4 bytes addr = 0x%X\n", addr);
						SANITY_ASSERT(0);
					}
				#endif

				#ifdef CONFIG_HOST_SYSTEM_LITTLE_ENDIAN
					#ifdef CONFIG_TARGET_SYSTEM_LITTLE_ENDIAN
						*((uint32_t*)pos) = data;
					#else
						jjjsjsdj
					#endif
				#else
					jjjsjsdj
				#endif
			}

			inline uint64_t read8(uint32_t addr) {
				volatile uint8_t *pos = this->vector + addr;
				uint64_t data;

				#ifdef SANITY_CHECK
					if ((addr+8) >= this->size) {
						LOG_PRINTF("memory read 8 bytes addr = 0x%X\n", addr);
						SANITY_ASSERT(0);
					}
				#endif

				#ifdef CONFIG_HOST_SYSTEM_LITTLE_ENDIAN
					#ifdef CONFIG_TARGET_SYSTEM_LITTLE_ENDIAN
						data = * ((uint64_t*)pos);
					#else
						jjjsjsdj
					#endif
				#else
					jjjsjsdj
				#endif
				return data;
			}

			inline void write8(uint32_t addr, uint64_t data) {
				volatile uint8_t *pos = this->vector + addr;

				#ifdef SANITY_CHECK
					if ((addr+8) >= this->size) {
						LOG_PRINTF("memory write 8 bytes addr = 0x%X\n", addr);
						SANITY_ASSERT(0);
					}
				#endif

				#ifdef CONFIG_HOST_SYSTEM_LITTLE_ENDIAN
					#ifdef CONFIG_TARGET_SYSTEM_LITTLE_ENDIAN
						*((uint64_t*)pos) = data;
					#else
						jjjsjsdj
					#endif
				#else
					jjjsjsdj
				#endif
			}

			inline void write(void *src, uint32_t n, uint32_t addr) {
				memcpy((void *)(this->vector + addr), src, n);
			}

			inline uint32_t get_size() {
				return this->size;
			}
	};

	class keyboard_t: public device_t
	{
		protected:
			uint8_t data;
			uint8_t state;
			uint8_t must_interrupt;

		public:
			keyboard_t();
			void cycle();

			// callbacks
			static void io_read_port_status(void *data, uint8_t size);
			static void io_read_port_data(void *data, uint8_t size);

			void interrupt_win();
			void interrupt_lost();
	};

	class int_controller_t
	{
		protected:
			/* devices that interrupted during cycle */
			device_t *devices[IO_NUMBER_DEVICES_THAT_INTERRUPT];
			cpu::processor_t *procs[IO_NUMBER_DEVICES_THAT_INTERRUPT];
			uint8_t codes[IO_NUMBER_DEVICES_THAT_INTERRUPT];

			uint32_t n_cpus;

			/* number of devices that interrupted during cycle */
			uint8_t n_int_devices;

		public:
			int_controller_t();

			void pre_cycle();
			void pos_cycle();
			void request_int(device_t *d, uint8_t code); // request interrupt from any cpu
			void request_int(device_t *d, uint8_t code, uint32_t cpuid); // request interrupt from cpu of id cpuid
	};

	class timer_t: public device_t
	{
		protected:
			uint64_t counter;
			uint8_t must_interrupt;
			uint64_t max_cycles;
			uint32_t n_cpus, cpu_to_interrupt;

		public:
			timer_t();

			void cycle();

			void interrupt_win();
			void interrupt_lost();
			
			inline void set_counter(uint64_t v) {
				this->counter = v;
			}
	};

	class external_hd_t: public device_t
	{
		protected:
			char file_name[100];

			/* first 5 bits indicate the state
			   last 3 bits indicate the status of last operation
			*/
			uint8_t state;

			uint8_t must_interrupt;

			FILE *file_handler;
			uint64_t fhandler;
			uint8_t file_buffer[IO_EXTERNAL_HD_SECTOR_SIZE];
			uint32_t file_buffer_count;
			uint32_t file_bytes, file_bytes_count, fhandler_count;
			uint32_t delay_cycles;
			uint32_t cfg_delay_cycles_read_per_byte;
			uint32_t cfg_delay_cycles_write_per_byte;

			inline uint32_t calculate_delay_cycle_read() {
				return this->cfg_delay_cycles_read_per_byte * this->file_bytes;
			}

			inline uint32_t calculate_delay_cycle_write() {
				return this->cfg_delay_cycles_write_per_byte * this->file_bytes;
			}

		public:
			external_hd_t();
			void cycle();

			// callbacks
			static void io_write_port_cmd(void *data, uint8_t size);
			static void io_read_port_status(void *data, uint8_t size);
			static void io_read_port_data(void *data, uint8_t size);
			static void io_write_port_data(void *data, uint8_t size);
			static void io_read_port_fbytes(void *data, uint8_t size);
			static void io_write_port_fbytes(void *data, uint8_t size);
			static void io_read_port_fhandler(void *data, uint8_t size);
			static void io_write_port_fhandler(void *data, uint8_t size);

			void interrupt_win();
			void interrupt_lost();
	};

	class video_t
	{
		protected:
			memory_t *mem;
			uint32_t buffer_addr, buffer_addr_count;
			uint32_t width, width_count, height, height_count;
			uint32_t cursor_pos_x, cursor_pos_x_count, cursor_pos_y, cursor_pos_y_count;

		public:
			video_t();

			void cycle();

			// callbacks
			static void io_write_port_set_buffer_addr(void *data, uint8_t size);
			static void io_write_port_set_width(void *data, uint8_t size);
			static void io_write_port_set_height(void *data, uint8_t size);
			static void io_write_port_set_cursor_pos_x(void *data, uint8_t size);
			static void io_write_port_set_cursor_pos_y(void *data, uint8_t size);
			static void io_write_port_cmd(void *data, uint8_t size);
	};

	void advancedss_io_initialize();
	memory_t* get_memory_obj();
	int_controller_t* get_int_controller();
	void register_io_callback_read(uint32_t pos, void (*callback)(void *data, uint8_t size));
	void register_io_callback_write(uint32_t pos, void (*callback)(void *data, uint8_t size));
	void call_io_handler_read(uint32_t pos, void *data, uint8_t size);
	void call_io_handler_write(uint32_t pos, void *data, uint8_t size);

	void cycle();

	extern video_t *video;
	extern memory_t *memory;
	extern keyboard_t *keyboard;
	extern external_hd_t *external_hd;
	extern int_controller_t *int_controller;
	extern timer_t *timer;
};

#endif
