// Little Polygon SDK
// Copyright (C) 2013 Max Kaufmann
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#pragma once
#include "Macros.h"

template<typename T, int N>
class Queue {
private:
	int n;
	int i;
	T buf[N];

public:
	Queue() : n(0), i(0) {}
	
	int capacity() const { return N; }
	int count() const { return n; }
	bool empty() const { return n == 0; }
	bool full() const { return n == N; }
	
	void enqueue(const T& val) {
		ASSERT(n < N);
		buf[(i + n) % N] = val;
		n++;
	}
	
	T dequeue() {
		ASSERT(n > 0);
		T result = buf[i];
		n--;
		i = (i+1) % N;
		return result;
	}

	T peekNext() {
		ASSERT(n > 0);
		return buf[i];
	}
	
	T peekLast() {
		ASSERT(n > 0);
		return buf[(i+n-1) % N];
	}
	
	bool tryDequeue(T* outValue) {
		if (n > 0) {
			*outValue = dequeue();
			return true;
		} else {
			return false;
		}
	}
	
	class Iterator {
	private:
		Queue *q;
		int idx;
		
	public:
		Iterator(Queue& queue) : q(&queue), idx(-1) {
		}
		
		bool step(T* outValue) {
			idx++;
			if (idx >= q->n) { return false; }
			*outValue = q->buf[(q->i + idx) % N];
			return true;
		}
	};
};
