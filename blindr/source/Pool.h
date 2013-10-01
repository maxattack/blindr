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

// This technique originally adapted from Niklas Frykholm's article in
// Game Developer magazine, November 2012.

#pragma once

#include "Macros.h"
#define MAX_CAPACITY (64*1024)

typedef uint32_t ID;

struct PoolSlot {
    ID id;
    uint16_t index;
    uint16_t next;
};


template<typename T, int N>
class Pool {
private:
    uint32_t mCount;
    T mRecords[N];
    PoolSlot mSlots[N];
    uint16_t mFreelistEnqueue;
    uint16_t mFreelistDequeue;
	
public:
    Pool() : mCount(0), mFreelistEnqueue(N-1), mFreelistDequeue(0) {
        ASSERT(N < MAX_CAPACITY);
        // initialize the free queue linked-list
        for(unsigned i=0; i<N; ++i) {
            mSlots[i].id = i;
            mSlots[i].next = i+1;
        }
    }
	
	bool isEmpty() const {
		return mCount == 0;
	}
	
	bool isFull() const {
		return mCount == N;
	}
	
    bool isActive(ID id) const {
        // use the lower-bits to find the record
        return mSlots[id & 0xffff].id == id;
    }
	
    T& operator[](ID id) {
        ASSERT(isActive(id)); 
        return mRecords[mSlots[id & 0xffff].index]; 
    }

    ID takeOut() {
        ASSERT(mCount < N);
        // dequeue a new index record - we do this in FIFO order so that
        // we don't "thrash" a record with interleaved add-remove calls
        // and use up the higher-order bits of the id
        PoolSlot &slot = mSlots[mFreelistDequeue];
		mFreelistDequeue = slot.next;
        // push a new record into the buffer
        slot.index = mCount++;
        // write the id to the record
        mRecords[slot.index].id = slot.id;
		return slot.id;
    }

    void putBack(ID id) {
        // assuming IDs are valid in production
        ASSERT(isActive(id));
        // lookup the index record
        PoolSlot &slot = mSlots[id & 0xffff];
		// move the last record into this slot
		T& record = mRecords[slot.index];
		record = mRecords[--mCount];
		// update the index from the moved record
		mSlots[record.id & 0xffff].index = slot.index;
        // increment the higher-order bits of the id (a fingerprint)
        slot.id += 0x10000;
		if (mCount == N-1) {
			mFreelistEnqueue = id & 0xffff;
			mFreelistDequeue = id & 0xffff;
		} else {
			mSlots[mFreelistEnqueue].next = id & 0xffff;
			mFreelistEnqueue = id & 0xffff;
		}
    }
    
	int count() const { return mCount; }

    uint16_t indexOf(ID id) { 
        return mSlots[id & 0xffff].index; 
    }

    T* begin() { 
        return mRecords; 
    }

    T* end() { 
        return mRecords + mCount; 
    }
};

// The anonymous pool is lighter weight than the regular pool because
// objects are not identified uniquely, but treated as a group.  Just don't
// hold on to references after takeOut()

template<typename T, int N>
class AnonymousPool {
private:
    uint32_t mCount;
    T mRecords[N];
	
public:
	AnonymousPool() : mCount(0) {
	}
	
	int count() const { return mCount; }
	bool isFull() const { return mCount == N; }
	T* begin() { return mRecords; }
	T* end() { return mRecords+mCount; }
	
	T* takeOut() {
		ASSERT(mCount < N);
		mCount++;
		return mRecords + (mCount-1);
	}
	
	void putBack(T* t) {
		int n = t - mRecords;
		ASSERT(n < mCount);
		mCount--;
		mRecords[n] = mRecords[mCount];
	}
	
	void drain() {
		mCount = 0;
	}
};

