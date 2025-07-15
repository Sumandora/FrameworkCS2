#pragma once

#include "../GameClass/MemAlloc.hpp"

template<typename T>
struct UtlVector {
	int size;
	T* elements;

	~UtlVector() {
		for(int i = 0; i < size; i++) {
			elements[i].~T();
		}
		MemAlloc::the()->deallocate(elements);
	}
};
