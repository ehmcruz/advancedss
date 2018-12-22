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

#ifndef _ADVANCEDSS_CONFIGUREFILE_HEADER_
#define _ADVANCEDSS_CONFIGUREFILE_HEADER_

#include <stdio.h>
#include "config.h"
#include "data/simple_linked.h"

struct advancedss_config_value {
	char *name;
	char *value;
};

typedef data_linked_list<advancedss_config_value, char*> advancedss_config_list;
typedef data_linked_el<advancedss_config_value, char*> advancedss_config_list_el;

class advancedss_config
{
	protected:
		FILE *fp;
		advancedss_config_list var_list;
		char file_name[50];

	public:
		advancedss_config();
		~advancedss_config();
		bool open(char *fname);
		void close();

		advancedss_config_value* search(char *var_name);

		int32_t read_int32(advancedss_config_value *v);
		double read_double(advancedss_config_value *v);
		char* read_str(advancedss_config_value *v);

		int32_t read_int32(char *var_name);
		double read_double(char *var_name);
		char* read_str(char *var_name);
};

advancedss_config* get_advancedss_main_config();

#endif
