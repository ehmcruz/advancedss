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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "config_file.h"
#include "config.h"

/***************************************************/

int8_t advancedss_config_value_keycmp(char *v1, char *v2);
char* advancedss_config_value_getkey(advancedss_config_value *v);
void advancedss_config_value_discard(advancedss_config_value *v);

/***************************************************/

static advancedss_config advancedss_main_config_file;

advancedss_config* get_advancedss_main_config()
{
	return &advancedss_main_config_file;
}

/***************************************************/

int8_t advancedss_config_value_keycmp(char *v1, char *v2)
{
	return strcmp(v1, v2);
}

char* advancedss_config_value_getkey(advancedss_config_value *v)
{
	return v->name;
}

void advancedss_config_value_discard(advancedss_config_value *v)
{
	free(v->name);
	free(v->value);
	free(v);
}

/***************************************************/

advancedss_config::advancedss_config()
{
	this->fp = NULL;
	this->var_list.set_getkey( advancedss_config_value_getkey );
	this->var_list.set_compare_keys( advancedss_config_value_keycmp );
	this->var_list.set_discard( advancedss_config_value_discard );
}

advancedss_config::~advancedss_config()
{
	if (this->fp)
		this->close();
}

// estados do autômato de interpretação
enum read_state { IDLE, RNAME, RVALUE };

bool advancedss_config::open(char *fname)
{
	char *buffer, *b, buffer2[100], *b2, buffer3[100];
	uint32_t size;
	read_state state;
	advancedss_config_value *variable;

	strcpy(this->file_name, fname);

	// primeiro, joga o arquivo na memória
	this->fp = fopen(fname, "rb");
	if (this->fp) {
		fseek(this->fp, 0, SEEK_END);
		size = ftell(this->fp);
		buffer = (char*) malloc(size+1);
		rewind(this->fp);
		fread(buffer, 1, size, this->fp);
		buffer[size] = 0; // end-of-string

		// inicia o autômato

		state = IDLE;
		b2 = buffer2;
		for (b=buffer; *b; b++) {
			switch (state) {
				case IDLE:
					/*if ((*b>='a' && *b <='z') || (*b>='A' && *b <='Z') || (*b>='0' && *b <='9') || *b=='_') {
						*b2 = *b;
						b2++;*/
					state = RNAME;
					//}
					//break;

				case RNAME:
					if ((*b>='a' && *b <='z') || (*b>='A' && *b <='Z') || (*b>='0' && *b <='9') || *b=='_') {
						*b2 = *b;
						b2++;
					}
					else if (*b == '=') {
						state = RVALUE;
						*b2 = 0;
						strcpy(buffer3, buffer2);
						b2 = buffer2;
					}
					break;

				case RVALUE:
					if (*b!=' ' && *b!=10 && *b!=13) {
						*b2 = *b;
						b2++;
					}
					else if (*b==10) {
						*b2 = 0;
						variable = (advancedss_config_value*) malloc ( sizeof(advancedss_config_value) );
						variable->name = (char*)malloc( strlen(buffer3) + 1 );
						variable->value = (char*)malloc( strlen(buffer2) + 1 );
						strcpy(variable->name, buffer3);
						strcpy(variable->value, buffer2);
//						DEBUG_PRINTF("%s -> %s\n", variable->name, variable->value);
						this->var_list.add_end(variable);
						state = IDLE;
						b2 = buffer2;
					}
					break;
			}
		}

		free(buffer);
		return true;
	}
	else
		return false;
}

void advancedss_config::close()
{
	fclose(this->fp);
}

int32_t advancedss_config::read_int32(char *var_name)
{
	advancedss_config_list_el *el;
	advancedss_config_value *data;

	el = this->var_list.search((char*)var_name);
	if (el) {
		data = el->get_data();
		return atoi( data->value );
	}
	else {
		LOG_PRINTF("Config-file %s: %s not found!", this->file_name, var_name);
		exit(1);
	}
}

double advancedss_config::read_double(char *var_name)
{
	advancedss_config_list_el *el;
	advancedss_config_value *data;

	el = this->var_list.search((char*)var_name);
	if (el) {
		data = el->get_data();
		return atof( data->value );
	}
	else
		return 0.0f;
}

char* advancedss_config::read_str(char *var_name)
{
	advancedss_config_list_el *el;
	advancedss_config_value *data;

	el = this->var_list.search((char*)var_name);
	if (el) {
		data = el->get_data();
		return data->value;
	}
	else {
		LOG_PRINTF("Config-file %s: %s not found!", this->file_name, var_name);
		exit(1);
	}
}

int32_t advancedss_config::read_int32(advancedss_config_value *v)
{
	return atoi(v->value);
}

double advancedss_config::read_double(advancedss_config_value *v)
{
	return atof(v->value);
}

char* advancedss_config::read_str(advancedss_config_value *v)
{
	return v->value;
}

advancedss_config_value* advancedss_config::search(char *var_name)
{
	advancedss_config_list_el *el;

	el = this->var_list.search((char*)var_name);
	if (el)
		return el->get_data();
	else
		return NULL;
}

