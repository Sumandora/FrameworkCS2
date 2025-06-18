#pragma once

#include "../VirtualMethod.hpp"

#include <cstddef>

class MemAlloc {
public:
	static MemAlloc* the();
	
	VIRTUAL_METHOD(2, allocate, void*, (std::size_t len), (this, len));
	VIRTUAL_METHOD(4, deallocate, void, (void* ptr), (this, ptr));
};
