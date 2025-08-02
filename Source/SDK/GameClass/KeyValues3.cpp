#include "KeyValues3.hpp"

#include "../../Libraries.hpp"

#include "../../Memory.hpp"

#include <dlfcn.h>

struct UtlString;

static void* (*load_kv3)(KeyValues3*, UtlString*, const char*, KV3ID const*, const char*, unsigned int) = nullptr;

void KeyValues3::resolve_functions()
{
	::load_kv3 = Libraries::tier0->get_symbol<decltype(::load_kv3)>("_Z7LoadKV3P10KeyValues3P10CUtlStringPKcRK7KV3ID_tS4_j");
	Memory::accept("LoadKV3", ::load_kv3);
}

void KeyValues3::load_kv3(const char* text, const KV3ID& kv3_id)
{
	::load_kv3(this, nullptr, text, &kv3_id, "", 0 /* TODO what is this int, it got added in an update */);
}
