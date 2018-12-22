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

#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#include "../host.h"
#include "../../main.h"

#define MAX_H   80
#define MAX_W   80

static char out_buffer[MAX_W];

static uint32_t w_ = 0, h_ = 0;

static volatile uint8_t input_buffer;
static volatile uint16_t has_input_buffer = 0;

static pthread_mutex_t mutex_input = PTHREAD_MUTEX_INITIALIZER;

static pthread_t thread_keyboard_input;

static clock_t start_time;

void* host_sim_input_thread_handler(void *v);

void host_sim_video_set_cursor_pos(uint32_t x, uint32_t y)
{
	move(y+1, x+1);
	refresh();
}

void host_sim_initialize()
{
	initscr();
	
	#ifdef CONFIG_LOG_TO_FILE
		#ifndef CONFIG_DO_NOT_WAIT_FOR_KEYBOARD_INPUT
			keypad(stdscr, TRUE);
			noecho();

			pthread_create(&thread_keyboard_input, NULL, host_sim_input_thread_handler, NULL);
		#endif
	#endif

	start_time = clock();
}

#ifdef CONFIG_DO_NOT_WAIT_FOR_KEYBOARD_INPUT
	void host_sim_keyboard_read_line(char *s, uint32_t n)
	{
		uint32_t len;
		getnstr(s, n-1);
		len = strlen(s);
		s[len] = 10;
		s[len+1] = 0;
	}
#endif

double host_sim_get_running_time()
{
	return (double)(clock() - start_time) / (double)CLOCKS_PER_SEC;
}

void* host_sim_input_thread_handler(void *v)
{
	#ifdef CONFIG_LOG_TO_FILE
		uint32_t c;

		while (advancedss_sim_running()) {
			c = getch();
//			ERROR_PRINTF("key %u typed (%u-%u)\n", c, KEY_F(0), KEY_F(12));
			pthread_mutex_lock(&mutex_input);
			switch (c) {
				case KEY_BACKSPACE:
					input_buffer = SIM_KEYBOARD_SCAN_CODE_BACKSPACE;
					break;
				
				case KEY_NPAGE:
					input_buffer = SIM_KEYBOARD_SCAN_CODE_PAGE_DOWN;
					break;
					
				case KEY_PPAGE:
					input_buffer = SIM_KEYBOARD_SCAN_CODE_PAGE_UP;
					break;
				
				default:
					if (c >= KEY_F(0) && c <= KEY_F(11))
						input_buffer = SIM_KEYBOARD_SCAN_CODE_F0 + c - KEY_F(0);
					else if (c < 128)
						input_buffer = c;
			}
			has_input_buffer = HOST_CODE_THERE_IS_INPUT;
			pthread_mutex_unlock(&mutex_input);
		}
	#endif
}

uint16_t host_sim_input_get_input_if_there_is()
{
	uint16_t d;

	pthread_mutex_lock(&mutex_input);
	d = input_buffer | has_input_buffer;
	has_input_buffer = 0;
	pthread_mutex_unlock(&mutex_input);

	return d;
}

void host_sim_video_refresh(uint8_t *buffer)
{
	uint32_t i, j, z, row;
	uint8_t c;

	row = 0;
	
	clear();

	row++;
	move(0, 0);
	for (j=0; j<w_+2; j++) {
		out_buffer[j] = '-';
	}
	out_buffer[j++] = 10;
	out_buffer[j] = 0;
	addstr(out_buffer);

	z = 0;
	for (i=0 ; i<h_; i++) {
		out_buffer[0] = '|';
		for (j=0; j<w_; j++) {
			c = buffer[z++];
			out_buffer[j+1] = (c >= 33 && c <= 126) ? c: ' ';
		}
		out_buffer[w_+1] = '|';
		out_buffer[w_+2] = 10;
		out_buffer[w_+3] = 0;
		addstr(out_buffer);
		row++;
	}

	for (j=0; j<w_+2; j++) {
		out_buffer[j] = '-';
	}
	out_buffer[j++] = 10;
	out_buffer[j] = 0;
	addstr(out_buffer);
			
	refresh();
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
	has_input_buffer = 0;

	if (h_ > MAX_H) {
			printf("height %u greater than max height\n", h_);
			exit(1);
	}

	if (w_ > MAX_W) {
			printf("width %u greater than max width\n", w_);
			exit(1);
	}
}
