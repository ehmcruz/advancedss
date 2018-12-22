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

#ifndef _ADVANCEDSS_DOUBLELINKEDSTATICLIST_HEADER_
#define _ADVANCEDSS_DOUBLELINKEDSTATICLIST_HEADER_

#include <stdlib.h>
#include <stdio.h>
#include "../config.h"

#define DATA_DBLINKED_STATIC_LIST_END       0xFFFFFFFF

// Double linked static list

template <class T, class TKEY>
class data_dblinked_static_el;

template <class T, class TKEY>
class data_dblinked_static_list
{
	protected:
		data_dblinked_static_el<T, TKEY> *list;

		int8_t (*compare)(T*, T*);
		int8_t (*compare_keys)(TKEY, TKEY);
		TKEY (*getkey)(T*);
		void (*discard)(T*);

		uint32_t n_elements, capacity, begin, end, empty;

	public:
		data_dblinked_static_list();
		~data_dblinked_static_list();

		void set_list_size(uint32_t n);

		inline uint32_t get_empty() {
			return this->capacity - this->n_elements;
		}

		inline data_dblinked_static_el<T, TKEY>* get_by_pos(uint32_t n) {
			return (n < this->capacity) ? this->list + n : NULL;
		}

		data_dblinked_static_el<T, TKEY>* add_end(T *el);
		data_dblinked_static_el<T, TKEY>* add_beginning(T *el);
		void destroy();
		data_dblinked_static_el<T, TKEY>* search(TKEY key);
		data_dblinked_static_el<T, TKEY>* search(T *el);

		// overload
		bool del(TKEY key);
		bool del(T *el);
		bool del(data_dblinked_static_el<T, TKEY> *el);

		data_dblinked_static_el<T, TKEY>* move_to_init(data_dblinked_static_el<T, TKEY> *el);

		bool del_first();
		bool del_last();

		inline uint32_t get_n_elements() {
			return this->n_elements;
		}

		inline uint32_t get_capacity() {
			return this->capacity;
		}

		inline data_dblinked_static_el<T, TKEY>* get_first() {
			return (this->n_elements != 0) ? this->list + this->begin : NULL;
		}

		inline data_dblinked_static_el<T, TKEY>* get_last() {
			return (this->n_elements != 0) ? this->list + this->end : NULL;
		}

		inline void set_compare(int8_t (*cmp)(T*, T*)) {
			this->compare = cmp;
		}

		inline void set_compare_keys(int8_t (*cmp_keys)(TKEY, TKEY)) {
			this->compare_keys = cmp_keys;
		}

		inline void set_getkey(TKEY (*key)(T*)) {
			this->getkey = key;
		}

		inline void set_discard(void (*dc)(T*)) {
			this->discard = dc;
		}
};

template <class T, class TKEY>
class data_dblinked_static_el {
	protected:
		T *data;
		uint32_t prev, next, next_empty, pos;
		data_dblinked_static_list<T, TKEY> *owner;

	public:
		inline void set_owner(data_dblinked_static_list<T, TKEY> *owner) {
			this->owner = owner;
		}

		inline T* get_data() {
			return this->data;
		}

		inline uint32_t get_next_pos() {
			return this->next;
		}

		inline uint32_t get_prev_pos() {
			return this->prev;
		}

		inline data_dblinked_static_el<T, TKEY>* get_next() {
			return this->owner->get_by_pos(this->next);
		}

		inline data_dblinked_static_el<T, TKEY>* get_prev() {
			return this->owner->get_by_pos(this->prev);
		}

		inline void set_data(T* data) {
			this->data = data;
		}

		inline void set_next (uint32_t next) {
			this->next = next;
		}

		inline void set_prev (uint32_t prev) {
			this->prev = prev;
		}

		inline void set_next_empty (uint32_t next_empty) {
			this->next_empty = next_empty;
		}

		inline uint32_t get_next_empty () {
			return this->next_empty;
		}

		inline void set_pos (uint32_t pos) {
			this->pos = pos;
		}

		inline uint32_t get_pos () {
			return this->pos;
		}
};

/********************************************************************/

template <class T, class TKEY>
data_dblinked_static_list<T, TKEY>::data_dblinked_static_list ()
{
	this->list = NULL;
	this->discard = NULL;
	this->getkey = NULL;
	this->compare_keys = NULL;
	this->compare = NULL;
	this->n_elements = 0;
	this->capacity = 0;
	this->begin = DATA_DBLINKED_STATIC_LIST_END;
	this->end = DATA_DBLINKED_STATIC_LIST_END;
}

template <class T, class TKEY>
void data_dblinked_static_list<T, TKEY>::set_list_size(uint32_t n)
{
	uint32_t i;

	if (n == 0) {
		LOG_PRINTF("error data_dblinked_static_list::set_list_size n=0\n");
		exit(1);
	}

	if (this->list)
		delete this->list;

	this->list = new data_dblinked_static_el<T, TKEY> [n];
	if (!this->list) {
		LOG_PRINTF("error malloc data_dblinked_static_list::set_list_size\n");
		exit(1);
	}

	for (i=0; i<n; i++) {
		this->list[i].set_owner( this );
		this->list[i].set_next_empty(i + 1);
		this->list[i].set_pos(i);
	}
	this->list[n-1].set_next_empty(DATA_DBLINKED_STATIC_LIST_END);

	this->capacity = n;
	this->begin = DATA_DBLINKED_STATIC_LIST_END;
	this->end = DATA_DBLINKED_STATIC_LIST_END;
	this->empty = 0;
	this->n_elements = 0;
}

template <class T, class TKEY>
data_dblinked_static_list<T, TKEY>::~data_dblinked_static_list ()
{
	this->destroy();
}

template <class T, class TKEY>
void data_dblinked_static_list<T, TKEY>::destroy ()
{
	uint32_t i;

	if (this->discard) {
		for (i=this->begin; i!=DATA_DBLINKED_STATIC_LIST_END; i=this->list[i].get_next_pos())
			(*this->discard)(this->list[i].get_data());
	}

	delete this->list;

	this->list = NULL;
	this->n_elements = 0;
	this->capacity = 0;
	this->begin = DATA_DBLINKED_STATIC_LIST_END;
	this->end = DATA_DBLINKED_STATIC_LIST_END;
}

template <class T, class TKEY>
data_dblinked_static_el<T, TKEY>* data_dblinked_static_list<T, TKEY>::add_end (T *el)
{
	uint32_t i;

	if (this->empty == DATA_DBLINKED_STATIC_LIST_END)
		return NULL;
	else if (this->n_elements == 0) {
		i = 0;
		this->begin = 0;
		this->end = 0;
		this->list[ i ].set_data( el );
		this->list[ i ].set_prev( DATA_DBLINKED_STATIC_LIST_END );
		this->list[ i ].set_next( DATA_DBLINKED_STATIC_LIST_END );

		this->empty = this->list[ i ].get_next_empty();
	}
	else {
		i = this->empty;

		this->list[ i ].set_data( el );
		this->list[ i ].set_prev( this->end );
		this->list[ i ].set_next( DATA_DBLINKED_STATIC_LIST_END );
		this->list[ this->end ].set_next( i );

		this->end = i;
		this->empty = this->list[ i ].get_next_empty();
	}

	this->n_elements++;
	return this->list + i;
}

template <class T, class TKEY>
data_dblinked_static_el<T, TKEY>* data_dblinked_static_list<T, TKEY>::add_beginning (T *el)
{
	uint32_t i;

	if (this->empty == DATA_DBLINKED_STATIC_LIST_END)
		return NULL;
	else if (this->n_elements == 0) {
		i = 0;
		this->begin = 0;
		this->end = 0;
		this->list[ i ].set_data( el );
		this->list[ i ].set_prev( DATA_DBLINKED_STATIC_LIST_END );
		this->list[ i ].set_next( DATA_DBLINKED_STATIC_LIST_END );

		this->empty = this->list[ i ].get_next_empty();
	}
	else {
		i = this->empty;

		this->list[ i ].set_data( el );
		this->list[ i ].set_prev( DATA_DBLINKED_STATIC_LIST_END );
		this->list[ i ].set_next( this->begin );
		this->list[ this->begin ].set_prev( i );

		this->begin = i;
		this->empty = this->list[ i ].get_next_empty();
	}

	this->n_elements++;
	return this->list + i;
}

template <class T, class TKEY>
data_dblinked_static_el<T, TKEY>* data_dblinked_static_list<T, TKEY>::search (TKEY key)
{
	uint32_t i, next;
	T *data;
	TKEY k;

	for (i=this->begin; i!=DATA_DBLINKED_STATIC_LIST_END; i=next) {
		next = this->list[i].get_next_pos();
		data = this->list[i].get_data();
		k = (*this->getkey)(data);
		if ( (*this->compare_keys)(key, k) == 0 )
			return this->list+i;
	}
	return NULL;
}

template <class T, class TKEY>
data_dblinked_static_el<T, TKEY>* data_dblinked_static_list<T, TKEY>::search (T *el)
{
	uint32_t i, next;
	T *data;

	for (i=this->begin; i!=DATA_DBLINKED_STATIC_LIST_END; i=next) {
		next = this->list[i].get_next_pos();
		data = this->list[i].get_data();
		if (el == data)
			return this->list+i;
	}
	return NULL;
}

template <class T, class TKEY>
bool data_dblinked_static_list<T, TKEY>::del(data_dblinked_static_el<T, TKEY> *el)
{
	uint32_t i;

	if (el->get_pos() == this->begin) {
		if (el->get_pos() == this->end) { // just one element
			this->begin = DATA_DBLINKED_STATIC_LIST_END;
			this->end = DATA_DBLINKED_STATIC_LIST_END;
			this->empty = 0;

			for (i=0; i<this->capacity; i++)
				this->list[i].set_next_empty(i + 1);
			this->list[this->capacity-1].set_next_empty(DATA_DBLINKED_STATIC_LIST_END);

			i = DATA_DBLINKED_STATIC_LIST_END;
		}
		else { // first
			i = this->begin;
			this->begin = this->list[ i ].get_next_pos();
			this->list[ this->begin ].set_prev(DATA_DBLINKED_STATIC_LIST_END);
		}
	}
	else {
		if (el->get_pos() == this->end) { // last
			i = this->end;
			this->end = this->list[ i ].get_prev_pos();
			this->list[ this->end ].set_next(DATA_DBLINKED_STATIC_LIST_END);
		}
		else { // middle
			uint32_t prev, next;
			i = el->get_pos();
			prev = el->get_prev_pos();
			next = el->get_next_pos();
			this->list[ prev ].set_next(next);
			this->list[ next ].set_prev(prev);
		}
	}

	// add "i" as next empty slot
	if (i != DATA_DBLINKED_STATIC_LIST_END) {
		this->list[ i ].set_next_empty(this->empty);
		this->empty = i;
	}

	if (this->discard)
		(*this->discard)(el->get_data());
	el->set_data(NULL);
	this->n_elements--;
	return true;
}

template <class T, class TKEY>
data_dblinked_static_el<T, TKEY>* data_dblinked_static_list<T, TKEY>::move_to_init(data_dblinked_static_el<T, TKEY> *el)
{
	T *data;

	data = el->get_data();
	this->del(el);
	return this->add_beginning(data);
}

template <class T, class TKEY>
bool data_dblinked_static_list<T, TKEY>::del (TKEY key)
{
	data_dblinked_static_el<T, TKEY> *el;

	el = this->search(key);
	if (el) {
		this->del(el);
		return true;
	}
	else
		return false;
}

template <class T, class TKEY>
bool data_dblinked_static_list<T, TKEY>::del (T *el)
{
	data_dblinked_static_el<T, TKEY> *e;

	e = this->search(el);
	if (e) {
		this->del(e);
		return true;
	}
	else
		return false;
}

template <class T, class TKEY>
bool data_dblinked_static_list<T, TKEY>::del_first()
{
	this->del( this->list + this->begin );
	return true;
}

template <class T, class TKEY>
bool data_dblinked_static_list<T, TKEY>::del_last()
{
	this->del( this->list + this->end );
	return true;
}

#endif
