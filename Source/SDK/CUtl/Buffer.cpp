#include "Buffer.hpp"

#include "../../Utils/Logging.hpp"

#include "../../SDK/GameClass/MemAlloc.hpp"

#include <cstring>
#include <dlfcn.h>

UtlBuffer::UtlBuffer(int grow_size, int init_size, int flags)
{
	memset(this, 0, sizeof(*this));
	static void (*ctor)(UtlBuffer*, int, int, int);

	if (!ctor) {
		void* handle = dlmopen(LM_ID_BASE, "libtier0.so", RTLD_NOW | RTLD_NOLOAD | RTLD_LOCAL);

		ctor = reinterpret_cast<decltype(ctor)>(dlsym(handle, "_ZN10CUtlBufferC1Eiii"));

		if (ctor)
			Logging::info("Found CUtlBuffer constructor at {}", reinterpret_cast<void*>(ctor));
		else
			Logging::error("Couldn't find global allocator!");

		dlclose(handle);
	}

	ctor(this, grow_size, init_size, flags);
}

UtlBuffer::~UtlBuffer()
{
	if (!memory.memory)
		return;

	if ((memory.growSize & 0xc0000000) == 0)
		MemAlloc::the()->deallocate(memory.memory);
}

void UtlBuffer::ensure_capacity(int size)
{
	static void (*fptr)(UtlBuffer*, int);

	if (!fptr) {
		void* handle = dlmopen(LM_ID_BASE, "libtier0.so", RTLD_NOW | RTLD_NOLOAD | RTLD_LOCAL);

		fptr = reinterpret_cast<decltype(fptr)>(dlsym(handle, "_ZN10CUtlBuffer14EnsureCapacityEi"));

		if (fptr)
			Logging::info("Found CUtlBuffer::EnsureCapacity at {}", reinterpret_cast<void*>(fptr));
		else
			Logging::error("Couldn't find CUtlBuffer::EnsureCapacity!");

		dlclose(handle);
	}

	fptr(this, size);
}
