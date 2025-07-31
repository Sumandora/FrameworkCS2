#include "KeyValues3.hpp"

#include "../../Libraries.hpp"

#include "../../Memory.hpp"

#include <dlfcn.h>

struct UtlString;

void KeyValues3::load_kv3(const char* text, const KV3ID& kv3_id)
{
	static auto fptr = [] {
		auto* symbol = Libraries::tier0->get_symbol<void* (*)(KeyValues3*, UtlString*, char const*, KV3ID const*, char const*, unsigned int)>("_Z7LoadKV3P10KeyValues3P10CUtlStringPKcRK7KV3ID_tS4_j");
		Memory::accept("LoadKV3", symbol);
		return symbol;
	}();

	fptr(this, nullptr, text, &kv3_id, "", 0 /* TODO what is this int, it got added in an update */);
}
