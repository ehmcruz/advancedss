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

#ifndef _ADVANCEDSS_DOUBLELINKEDLIST_HEADER_
#define _ADVANCEDSS_DOUBLELINKEDLIST_HEADER_

#include <stdlib.h>
#include "../config.h"

// Double linked list

template <class T, class TKEY>
class data_dblinked_el {
	protected:
		T *data;
		data_dblinked_el<T, TKEY> *prev, *next;

	public:
		inline T* get_data() {
			return this->data;
		}

		inline data_dblinked_el<T, TKEY>* get_next() {
			return this->next;
		}

		inline data_dblinked_el<T, TKEY>* get_prev() {
			return this->prev;
		}

		inline void set_data(T* data) {
			this->data = data;
		}

		inline void set_next (data_dblinked_el<T, TKEY> *next) {
			this->next = next;
		}

		inline void set_prev (data_dblinked_el<T, TKEY> *prev) {
			this->prev = prev;
		}
};

template <class T, class TKEY>
class data_dblinked_list
{
	protected:
		data_dblinked_el<T, TKEY> *list, *end;

		int8_t (*compare)(T*, T*);
		int8_t (*compare_keys)(TKEY, TKEY);
		TKEY (*getkey)(T*);
		void (*discard)(T*);

		uint32_t n_elements;

	public:
		data_dblinked_list();
		~data_dblinked_list();
		data_dblinked_el<T, TKEY>* add_end(T *el);
		data_dblinked_el<T, TKEY>* add_beginning(T *el);
		void destroy();
		data_dblinked_el<T, TKEY>* search(TKEY key);
		data_dblinked_el<T, TKEY>* get_el_by_pos(uint32_t pos);

		// overload
		bool del(TKEY key);
		bool del(T *el);
		bool del(data_dblinked_el<T, TKEY> *el);

		bool move_to_top(data_dblinked_el<T, TKEY> *el);

		bool del_first();
		bool del_last();

		inline uint32_t get_n_elements() {
			return this->n_elements;
		}

		inline data_dblinked_el<T, TKEY>* get_first() {
			return this->list;
		}

		inline data_dblinked_el<T, TKEY>* get_last() {
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
};

/********************************************************************/

template <class T, class TKEY>
data_dblinked_list<T, TKEY>::data_dblinked_list ()
{
	this->list = NULL;
	this->end = NULL;
	this->discard = NULL;
	this->n_elements = 0;
}

template <class T, class TKEY>
data_dblinked_list<T, TKEY>::~data_dblinked_list ()
{
	this->destroy();
}

template <class T, class TKEY>
void data_dblinked_list<T, TKEY>::destroy ()
{
	data_dblinked_el<T, TKEY> *next;

	while (this->list) {
		next = this->list->get_next();
		if (this->discard)
			(*this->discard)(this->list->get_data());
		delete this->list;
		this->list = next;
	}
	this->list = NULL;
	this->end = NULL;
	this->n_elements = 0;
}

template <class T, class TKEY>
data_dblinked_el<T, TKEY>* data_dblinked_list<T, TKEY>::add_end (T *el)
{
	data_dblinked_el<T, TKEY> *tmp;

	tmp = new data_dblinked_el<T, TKEY>;
	tmp->set_next(NULL);
	tmp->set_prev(this->end);
	tmp->set_data(el);

	if (!this->list) {
		this->list = tmp;
		this->end = tmp;
	}
	else {
		this->end->set_next(tmp);
		this->end = tmp;
	}

	this->n_elements++;
	return tmp;
}

template <class T, class TKEY>
data_dblinked_el<T, TKEY>* data_dblinked_list<T, TKEY>::add_beginning (T *el)
{
	data_dblinked_el<T, TKEY> *tmp;

	tmp = new data_dblinked_el<T, TKEY>;
	tmp->set_next(this->list);
	tmp->set_prev(NULL);
	tmp->set_data(el);

	if (!this->end) // first element
		this->end = tmp;
	else
		this->list->set_prev(tmp);

	this->list = tmp;
	this->n_elements++;
	return tmp;
}

template <class T, class TKEY>
data_dblinked_el<T, TKEY>* data_dblinked_list<T, TKEY>::search (TKEY key)
{
	data_dblinked_el<T, TKEY> *p;
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
data_dblinked_el<T, TKEY>* data_dblinked_list<T, TKEY>::get_el_by_pos(uint32_t pos)
{
	data_dblinked_el<T, TKEY> *p;

	for (p=this->get_first(); p!=NULL && pos>0; p=p->get_next()) {
		pos--;
	}
	return p;
}

template <class T, class TKEY>
bool data_dblinked_list<T, TKEY>::del(data_dblinked_el<T, TKEY> *el)
{
	if (el == this->list) {
		if (el == this->end) {
			this->list = this->end = NULL;
		}
		else {
			this->list = this->list->get_next();
			this->list->set_prev(NULL);
		}
	}
	else {
		if (el == this->end) {
			this->end = el->get_prev();
			this->end->set_next(NULL);
		}
		else {
			data_dblinked_el<T, TKEY> *prev, *next;
			prev = el->get_prev();
			next = el->get_next();
			prev->set_next(next);
			next->set_prev(prev);
		}
	}

	if (this->discard)
		(*this->discard)(el->get_data());
	delete el;
	this->n_elements--;
	return true;
}

template <class T, class TKEY>
bool data_dblinked_list<T, TKEY>::del (TKEY key)
{
	data_dblinked_el<T, TKEY> *el;

	el = this->search(key);
	if (el) {
		this->del(el);
		return true;
	}
	else
		return false;
}

template <class T, class TKEY>
bool data_dblinked_list<T, TKEY>::del (T *el)
{
	data_dblinked_el<T, TKEY> *p;
	T *data;

	for (p=this->get_first(); p!=NULL; p=p->get_next()) {
		data = p->get_data();
		if ( data == el ) {
			this->del(p);
			return true;
		}
	}

	return false;
}

template <class T, class TKEY>
bool data_dblinked_list<T, TKEY>::del_first()
{
	data_dblinked_el<T, TKEY> *el;

	el = this->list;
	if (this->list == this->end) {
		this->list = this->end = NULL;
	}
	else {
		this->list = el->get_next();
		this->list->set_prev(NULL);
	}

	if (this->discard)
		(*this->discard)(el->get_data());
	delete el;
	this->n_elements--;
	return true;
}

template <class T, class TKEY>
bool data_dblinked_list<T, TKEY>::del_last()
{
	data_dblinked_el<T, TKEY> *el;

	el = this->end;
	if (this->list == this->end) {
		this->list = this->end = NULL;
	}
	else {
		this->end = el->get_prev();
		this->end->set_next(NULL);
	}

	if (this->discard)
		(*this->discard)(el->get_data());
	delete el;
	this->n_elements--;
	return true;
}

template <class T, class TKEY>
bool data_dblinked_list<T, TKEY>::move_to_top(data_dblinked_el<T, TKEY> *el)
{
	if (el != this->end) { // not on top
		if (el == this->list) { // in the beggining of the list
			this->list = this->list->get_next();
			this->list->set_prev( NULL );
		}
		else {
			data_dblinked_el<T, TKEY> *next, *prev;
			next = el->get_next();
			prev = el->get_prev();
			next->set_prev( prev );
			prev->set_next( next );
		}

		this->end->set_next( el );
		el->set_next( NULL );
		el->set_prev( this->end );
		this->end = el;
	}
}

#endif
