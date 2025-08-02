#include "Buffer.hpp"

#include "../../SDK/GameClass/MemAlloc.hpp"

#include "../../Libraries.hpp"
#include "../../Memory.hpp"

#include "RetAddrSpoofer.hpp"

#include <cstring>
#include <dlfcn.h>

static void (*ctor)(UtlBuffer*, int, int, BufferFlags) = nullptr;
static void (*ensure_capacity)(UtlBuffer*, int) = nullptr;

void UtlBuffer::resolve_functions()
{
	ctor = Libraries::tier0->get_symbol<decltype(ctor)>("_ZN10CUtlBufferC1EiiNS_13BufferFlags_tE");
	MEM_ACCEPT(ctor);
	::ensure_capacity = Libraries::tier0->get_symbol<decltype(::ensure_capacity)>("_ZN10CUtlBuffer14EnsureCapacityEi");
	Memory::accept("CUtlBuffer::EnsureCapacity", ::ensure_capacity);
}

UtlBuffer::UtlBuffer(int grow_size, int init_size, BufferFlags flags)
{
	std::memset(reinterpret_cast<void*>(this), 0, sizeof(*this));
	RetAddrSpoofer::invoke(ctor, this, grow_size, init_size, flags);
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
	RetAddrSpoofer::invoke(::ensure_capacity, this, size);
}
