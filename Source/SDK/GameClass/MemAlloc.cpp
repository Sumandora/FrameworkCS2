#include "MemAlloc.hpp"

#include "../../Utils/Logging.hpp"

#include "../../Libraries.hpp"

#include <dlfcn.h>

MemAlloc* MemAlloc::the()
{
	static MemAlloc** global_allocator = [] {
		auto** global_allocator = Libraries::tier0->get_symbol<MemAlloc**>("g_pMemAlloc");

		if (global_allocator)
			Logging::info("Found global allocator at {} => {}", global_allocator, *global_allocator);
		else
			Logging::error("Couldn't find global allocator!");

		return global_allocator;
	}();

	return *global_allocator;
}
