#include "MemAlloc.hpp"

#include "../../Utils/Logging.hpp"

#include <dlfcn.h>

MemAlloc* MemAlloc::the()
{
	static MemAlloc** global_allocator;

	if (!global_allocator) {
		void* handle = dlmopen(LM_ID_BASE, "libtier0.so", RTLD_NOW | RTLD_NOLOAD | RTLD_LOCAL);

		global_allocator = static_cast<MemAlloc**>(dlsym(handle, "g_pMemAlloc"));

		if (global_allocator)
			Logging::info("Found global allocator at {} => {}", global_allocator, *global_allocator);
		else
			Logging::error("Couldn't find global allocator!");

		dlclose(handle);
	}

	return *global_allocator;
}
