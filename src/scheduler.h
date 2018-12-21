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

    January 2010
*/

#ifndef _ADVANCEDSS_SCHEDULER_HEADER_
#define _ADVANCEDSS_SCHEDULER_HEADER_

#include <queue>
#include "config.h"

template <typename T, typename TPAR>
struct scheduler_el_t {
	T *handler;
	TPAR par;
	void (T::*callback)(TPAR);
	uint64_t count;
};

template <typename T, typename TPAR>
class scheduler_t
{
	protected:
		std::priority_queue< scheduler_el_t<T, TPAR> > pqueue;
		uint64_t count;

	public:
		scheduler_t(uint64_t count) {
			this->count = count;
		}

		void schedule(T *handler, void (T::*callback)(TPAR), TPAR par, uint64_t time) {
			scheduler_el_t<T, TPAR> el;
			el. handler = handler;
			el.callback = callback;
			el.count = this->count + time;
			el.par = par;
			this->pqueue.push( el );
		}

		void cycle() {
			while (!this->pqueue.empty()) {
				if (this->pqueue.top().count >= this->count) {
					((this->pqueue.top().handler) ->* (this->pqueue.top().callback)) (this->pqueue.top().par);
					this->pqueue.pop();
				}
				else
					break;
			}

			this->count++;
		}
};

#endif
