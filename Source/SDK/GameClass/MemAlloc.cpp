#include "MemAlloc.hpp"

#include "../../Memory.hpp"

#include "../../Libraries.hpp"

#include <dlfcn.h>

static MemAlloc** global_allocator = nullptr;

void MemAlloc::resolve_signatures()
{
	global_allocator = Libraries::tier0->get_symbol<MemAlloc**>("g_pMemAlloc");

	MEM_ACCEPT(global_allocator);
}

MemAlloc* MemAlloc::the()
{
	return *global_allocator;
}
