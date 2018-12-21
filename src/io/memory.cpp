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

io::memory_t::memory_t()
{
	advancedss_config *cfg = get_advancedss_main_config();
	uint32_t size = cfg->read_int32("memory_size");
	this->vector = (uint8_t*)malloc( size );
	if (this->vector == NULL) {
		LOG_PRINTF("io::memory_t fail malloc memory\n");
		exit(1);
	}
	memset((void*)this->vector, 0, size);
	this->size = size;
	pthread_mutex_init(&this->mutex, NULL);
	pthread_cond_init(&this->cond_memory_lock, NULL);
	pthread_cond_init(&this->cond_memory_lockfree, NULL);
	this->memory_is_locked = 0;
	this->memory_lockfree_operations_running = 0;
	this->lockfree_operations_pending = 0;
	this->lock_operations_pending = 0;
}

void io::memory_t::lock()
{
	pthread_mutex_lock(&this->mutex);
	while (this->memory_is_locked || this->memory_lockfree_operations_running) {
		this->lock_operations_pending++;
		pthread_cond_wait( &this->cond_memory_lock, &this->mutex );
		this->lock_operations_pending--;
	}
	this->memory_is_locked = 1;
	pthread_mutex_unlock(&this->mutex);
}

void io::memory_t::unlock()
{
	pthread_mutex_lock(&this->mutex);
	this->memory_is_locked = 0;
	if (this->lock_operations_pending) {
		pthread_cond_signal(&this->cond_memory_lock);
	}
	else if (this->lockfree_operations_pending) {
		pthread_cond_broadcast(&this->cond_memory_lockfree);
	}
	pthread_mutex_unlock(&this->mutex);
}

void io::memory_t::check_lock()
{
	pthread_mutex_lock(&this->mutex);
	while (this->memory_is_locked) {
		this->lockfree_operations_pending++;
		pthread_cond_wait( &this->cond_memory_lockfree, &this->mutex );
		this->lockfree_operations_pending--;
	}
	this->memory_lockfree_operations_running++;
	pthread_mutex_unlock(&this->mutex);
}

void io::memory_t::check_unlock()
{
	pthread_mutex_lock(&this->mutex);
	this->memory_lockfree_operations_running--;
	if (this->lock_operations_pending && this->memory_lockfree_operations_running == 0) {
		pthread_cond_signal(&this->cond_memory_lock);
	}
	pthread_mutex_unlock(&this->mutex);
}
