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

#ifndef _ADVANCEDSS_SIMPLELINKEDLIST_HEADER_
#define _ADVANCEDSS_SIMPLELINKEDLIST_HEADER_

#include <stdlib.h>
#include "../config.h"

// Simple linked list

template <class T, class TKEY>
class data_linked_el {
	protected:
		T *data;
		data_linked_el<T, TKEY> *next;

	public:
		inline T* get_data() {
			return this->data;
		}

		inline data_linked_el<T, TKEY>* get_next() {
			return this->next;
		}

		inline void set_data(T *data) {
			this->data = data;
		}

		inline void set_next(data_linked_el<T, TKEY> *next) {
			this->next = next;
		}
};

template <class T, class TKEY>
class data_linked_list
{
	protected:
		data_linked_el<T, TKEY> *list, *end;

		int8_t (*compare)(T*, T*);
		int8_t (*compare_keys)(TKEY, TKEY);
		TKEY (*getkey)(T*);
		void (*discard)(T*);
		uint32_t n_elements;
		void (*print_el)(T*);

	public:
		data_linked_list();
		~data_linked_list();
		data_linked_el<T, TKEY>* add_end(T *el);
		data_linked_el<T, TKEY>* add_beginning(T *el);
		void destroy();
		data_linked_el<T, TKEY>* search(TKEY key);

		// overload
		bool del(TKEY key);
		bool del(data_linked_el<T, TKEY> *el, data_linked_el<T, TKEY> *prev);

		bool del_first();
		bool del_last();

		void put_begin_in_end();

		void print_list();

		inline data_linked_el<T, TKEY>* add(T *el) {
			return this->add_end(el);
		}

		inline uint32_t get_n_elements() {
			return this->n_elements;
		}

		inline data_linked_el<T, TKEY>* get_first() {
			return this->list;
		}

		inline data_linked_el<T, TKEY>* get_last() {
			return this->end;
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

		inline void set_print_el(void (*pe)(T*)) {
			this->print_el = pe;
		}
};

/********************************************************************/

template <class T, class TKEY>
data_linked_list<T, TKEY>::data_linked_list ()
{
	this->list = this->end = NULL;
	this->discard = NULL;
	this->n_elements = 0;
}

template <class T, class TKEY>
data_linked_list<T, TKEY>::~data_linked_list ()
{
	this->destroy();
}

template <class T, class TKEY>
void data_linked_list<T, TKEY>::destroy ()
{
	data_linked_el<T, TKEY> *next;

	while (this->list) {
		next = this->list->get_next();
		if (this->discard)
			(*this->discard)(this->list->get_data());
		delete this->list;
		this->list = next;
	}
	this->n_elements = 0;
}

template <class T, class TKEY>
data_linked_el<T, TKEY>* data_linked_list<T, TKEY>::add_end (T *el)
{
	data_linked_el<T, TKEY> *tmp;

	tmp = new data_linked_el<T, TKEY>;
	tmp->set_next(NULL);
	tmp->set_data(el);

	if (!this->list)
		this->list = this->end = tmp;
	else {
		this->end->set_next(tmp);
		this->end = tmp;
	}
	this->n_elements++;
	return tmp;
}

template <class T, class TKEY>
data_linked_el<T, TKEY>* data_linked_list<T, TKEY>::add_beginning (T *el)
{
	data_linked_el<T, TKEY> *tmp;

	tmp = new data_linked_el<T, TKEY>;
	tmp->set_next(this->list);
	tmp->set_data(el);
	this->list = tmp;

	if (!this->end) // first element
		this->end = tmp;
	this->n_elements++;
	return tmp;
}

template <class T, class TKEY>
data_linked_el<T, TKEY>* data_linked_list<T, TKEY>::search (TKEY key)
{
	data_linked_el<T, TKEY> *p;
	T *data;
	TKEY k;

	for (p=this->get_first(); p!=NULL; p=p->get_next()) {
		data = p->get_data();
		k = (*this->getkey)(data);
		if ( (*this->compare_keys)(key, k) == 0 )
			return p;
	}
	return NULL;
}

template <class T, class TKEY>
bool data_linked_list<T, TKEY>::del(data_linked_el<T, TKEY> *el, data_linked_el<T, TKEY> *prev)
{
	if (el == this->list) {
		if (el == this->end) {
			this->list = this->end = NULL;
		}
		else {
			this->list = this->list->get_next();
		}
	}
	else {
		if (el == this->end) {
			this->end = prev;
			prev->set_next(NULL);
		}
		else {
			prev->set_next( el->get_next() );
		}
	}

	if (this->discard)
		(*this->discard)(el->get_data());
	delete el;
	this->n_elements--;
	return true;
}

template <class T, class TKEY>
bool data_linked_list<T, TKEY>::del (TKEY key)
{
	data_linked_el<T, TKEY> *el, *prev, *p;
	T *data;
	TKEY k;

	prev = el = NULL;

	p = this->get_first();
	while (p != NULL) {
		data = p->get_data();
		k = (*this->getkey)(data);
		if ( (*this->compare_keys)(key, k) == 0 ) {
			el = p;
			p = NULL;
		}
		else {
			prev = p;
			p = p->get_next();
		}
	}

	if (el) {
		this->del(el, prev);
		return true;
	}
	else
		return false;
}

template <class T, class TKEY>
bool data_linked_list<T, TKEY>::del_first()
{
	data_linked_el<T, TKEY> *el;

	el = this->list;
	if (this->list == this->end) {
		this->list = this->end = NULL;
	}
	else {
		this->list = el->get_next();
	}

	if (this->discard)
		(*this->discard)(el->get_data());
	delete el;
	this->n_elements--;
	return true;
}

template <class T, class TKEY>
bool data_linked_list<T, TKEY>::del_last()
{
	data_linked_el<T, TKEY> *el, *prev, *next;

	el = this->end;
	if (this->list == this->end) {
		this->list = this->end = NULL;
	}
	else {
		prev = this->list;
		next = prev->get_next();
		while (next != this->end) {
			prev = next;
			next = next->get_next();
		}
		this->end = prev;
		this->end->next = NULL;
	}

	if (this->discard)
		(*this->discard)(el->get_data());
	delete el;
	this->n_elements--;
	return true;
}

template <class T, class TKEY>
void data_linked_list<T, TKEY>::put_begin_in_end()
{
	data_linked_el<T, TKEY> *el;

	if ( (this->list != NULL) && (this->n_elements > 1) ) {
		el = this->list;
		this->list = el->get_next();
		el->set_next(NULL);
		this->end->set_next(el);
		this->end = el;
	}
}

template <class T, class TKEY>
void data_linked_list<T, TKEY>::print_list()
{
	data_linked_el<T, TKEY> *el;
	T *data;
	TKEY k;

	el = this->list;
	if ( el != NULL ) {
		{
			data = el->get_data();
			k  = (*this->getkey)(data);
			(*this->print_el)(data);
			el = el->get_next();
		} while ( el->get_next() != NULL );
	}
}

#endif
