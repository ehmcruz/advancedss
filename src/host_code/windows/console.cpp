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

#include <windows.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include "../host.h"
#include "../../main.h"

#define MAX_H   45
#define MAX_W   80

static char out_buffer[MAX_W];

static uint32_t w_ = 0, h_ = 0;

static volatile uint8_t input_buffer;
static volatile uint16_t has_input_buffer = 0;

static pthread_mutex_t mutex_input = PTHREAD_MUTEX_INITIALIZER;

static pthread_t thread_keyboard_input;

static clock_t start_time;

void* host_sim_input_thread_handler(void *v);

void host_sim_initialize()
{
	#ifdef CONFIG_LOG_TO_FILE
		#ifndef CONFIG_DO_NOT_WAIT_FOR_KEYBOARD_INPUT
			HANDLE console_in = GetStdHandle(STD_INPUT_HANDLE);

			SetConsoleMode(console_in, 0);

			pthread_create(&thread_keyboard_input, NULL, host_sim_input_thread_handler, NULL);
		#endif
	#endif

	start_time = clock();
}

#ifdef CONFIG_DO_NOT_WAIT_FOR_KEYBOARD_INPUT
	void host_sim_keyboard_read_line(char *s, uint32_t n)
	{
		gets(s);
	}
#endif

void host_sim_video_set_cursor_pos(uint32_t x, uint32_t y)
{
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD cursor_pos;

	cursor_pos.X = x+1;
	cursor_pos.Y = y+1;
	SetConsoleCursorPosition(console, cursor_pos);
}

double host_sim_get_running_time()
{
	return (double)(clock() - start_time) / (double)CLOCKS_PER_SEC;
}

void* host_sim_input_thread_handler(void *v)
{
	#ifdef CONFIG_LOG_TO_FILE
		INPUT_RECORD console_input;
		CONSOLE_SCREEN_BUFFER_INFO console_info;
		HANDLE console = GetStdHandle(STD_INPUT_HANDLE);
		DWORD n;

		GetConsoleScreenBufferInfo(console, &console_info);

		while (advancedss_sim_running()) {
			ReadConsoleInput(console, &console_input, 1, &n);
			if (console_input.EventType == KEY_EVENT && console_input.Event.KeyEvent.bKeyDown) {
				//LOG_PRINTF("key typed %u\n", (uint32_t)c);
				pthread_mutex_lock(&mutex_input);
				switch (console_input.Event.KeyEvent.wVirtualKeyCode) {
					case VK_RETURN:
						input_buffer = SIM_KEYBOARD_SCAN_CODE_NEWLINE;
						break;
					case VK_PRIOR:
						input_buffer = SIM_KEYBOARD_SCAN_CODE_PAGE_UP;
						break;
					case VK_NEXT:
						input_buffer = SIM_KEYBOARD_SCAN_CODE_PAGE_DOWN;
						break;
					default:
						if (console_input.Event.KeyEvent.uChar.AsciiChar < 128)
							input_buffer = console_input.Event.KeyEvent.uChar.AsciiChar;
				}
				has_input_buffer = HOST_CODE_THERE_IS_INPUT;
				pthread_mutex_unlock(&mutex_input);
			}
		}
	#endif

	return NULL;
}

uint16_t host_sim_input_get_input_if_there_is()
{
	uint16_t d;

	pthread_mutex_lock(&mutex_input);
	d = input_buffer | has_input_buffer;
	pthread_mutex_unlock(&mutex_input);

	has_input_buffer = 0;
	return d;
}

void host_sim_video_refresh(uint8_t *buffer)
{
	uint32_t i, j, z, row;
	DWORD written;
	uint8_t c;
	CONSOLE_SCREEN_BUFFER_INFO console_info;
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD cursor_pos;

	GetConsoleScreenBufferInfo(console, &console_info);

	row = 0;

	cursor_pos.X = 0;
	cursor_pos.Y = row++;
	SetConsoleCursorPosition(console, cursor_pos);
	for (j=0; j<w_+2; j++) {
		out_buffer[j] = '-';
	}
	WriteConsole(console, out_buffer, w_+2, &written, NULL);

	z = 0;
	for (i=0 ; i<h_; i++) {
		cursor_pos.X = 0;
		cursor_pos.Y = row++;
		SetConsoleCursorPosition(console, cursor_pos);
		out_buffer[0] = '|';
		for (j=0; j<w_; j++) {
			c = buffer[z++];
			out_buffer[j+1] = (c >= 33 && c <= 126) ? c: ' ';
		}
		out_buffer[w_+1] = '|';
		WriteConsole(console, out_buffer, w_+2, &written, NULL);
	}

	cursor_pos.X = 0;
	cursor_pos.Y = row++;
	SetConsoleCursorPosition(console, cursor_pos);
	for (j=0; j<w_+2; j++) {
		out_buffer[j] = '-';
	}
	WriteConsole(console, out_buffer, w_+2, &written, NULL);
}

void host_sim_video_set_height(uint32_t h)
{
	h_ = h;
}

void host_sim_video_set_width(uint32_t w)
{
	w_ = w;
}

void host_sim_video_start()
{
	CONSOLE_SCREEN_BUFFER_INFO console_info_out;
	CONSOLE_SCREEN_BUFFER_INFO console_info_in;
	HANDLE console_out = GetStdHandle(STD_OUTPUT_HANDLE);

	has_input_buffer = 0;

	GetConsoleScreenBufferInfo(console_out, &console_info_out);

	//printf("max screen size is w=%u h=%u\n", console_info.dwSize.X, console_info.dwSize.Y);

	if (h_ > MAX_H) {
			printf("height %u greater than max height\n", h_);
			exit(1);
	}

	if (w_ > MAX_W) {
			printf("width %u greater than max width\n", w_);
			exit(1);
	}
}
