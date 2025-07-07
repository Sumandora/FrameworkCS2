#include "KeyValues3.hpp"

#include "../../Utils/Logging.hpp"

#include <dlfcn.h>

struct UtlString;

void KeyValues3::load_kv3(const char* text, const KV3ID& kv3_id)
{
	static void* (*fptr)(KeyValues3*, UtlString*, char const*, KV3ID const*, char const*);

	if (!fptr) {
		void* handle = dlmopen(LM_ID_BASE, "libtier0.so", RTLD_NOW | RTLD_NOLOAD | RTLD_LOCAL);

		fptr = reinterpret_cast<decltype(fptr)>(dlsym(handle, "_Z7LoadKV3P10KeyValues3P10CUtlStringPKcRK7KV3ID_tS4_"));

		if (fptr)
			Logging::info("Found LoadKV3 at {}", reinterpret_cast<void*>(fptr));
		else
			Logging::error("Couldn't find LoadKV3!");

		dlclose(handle);
	}

	fptr(this, nullptr, text, &kv3_id, "");
}
