#include "Buffer.hpp"

#include "../../Utils/Logging.hpp"

#include "../../SDK/GameClass/MemAlloc.hpp"

#include "../../Libraries.hpp"

#include <cstring>
#include <dlfcn.h>

UtlBuffer::UtlBuffer(int grow_size, int init_size, BufferFlags flags)
{
	std::memset(reinterpret_cast<void*>(this), 0, sizeof(*this));
	static auto ctor = [] {
		auto* symbol = Libraries::tier0->get_symbol<void (*)(UtlBuffer*, int, int, BufferFlags)>("_ZN10CUtlBufferC1EiiNS_13BufferFlags_tE");

		if (symbol)
			Logging::info("Found CUtlBuffer constructor at {}", reinterpret_cast<void*>(symbol));
		else
			Logging::error("Couldn't find CUtlBuffer constructor!");

		return symbol;
	}();

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
	static auto fptr = [] {
		auto* symbol = Libraries::tier0->get_symbol<void (*)(UtlBuffer*, int)>("_ZN10CUtlBuffer14EnsureCapacityEi");

		if (symbol)
			Logging::info("Found CUtlBuffer::EnsureCapacity at {}", reinterpret_cast<void*>(symbol));
		else
			Logging::error("Couldn't find CUtlBuffer::EnsureCapacity!");

		return symbol;
	}();

	fptr(this, size);
}
