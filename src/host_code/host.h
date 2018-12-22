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

#ifndef _ADVANCEDSS_HOSTCODE_HEADER_
#define _ADVANCEDSS_HOSTCODE_HEADER_

#include "../config.h"

#define HOST_CODE_THERE_IS_INPUT     0x8000
#define HOST_CODE_GET_INPUT(R)       (R & 0x00FF)

uint16_t host_sim_input_get_input_if_there_is();

void host_sim_initialize();

void host_sim_video_refresh(uint8_t *buffer);
void host_sim_video_set_height(uint32_t h);
void host_sim_video_set_width(uint32_t w);
void host_sim_video_set_cursor_pos(uint32_t x, uint32_t y);
void host_sim_video_start();
//double host_sim_get_running_time();

#ifdef CONFIG_DO_NOT_WAIT_FOR_KEYBOARD_INPUT
	void host_sim_keyboard_read_line(char *s, uint32_t n);
#endif

#define SIM_KEYBOARD_SCAN_CODE_NEWLINE          10
#define SIM_KEYBOARD_SCAN_CODE_BACKSPACE        8
#define SIM_KEYBOARD_SCAN_CODE_F0               128
#define SIM_KEYBOARD_SCAN_CODE_F12              (128+12)
#define SIM_KEYBOARD_SCAN_CODE_PAGE_UP          141
#define SIM_KEYBOARD_SCAN_CODE_PAGE_DOWN        142

#endif
