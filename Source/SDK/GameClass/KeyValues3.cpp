#include "KeyValues3.hpp"

#include "../../Utils/Logging.hpp"

#include "../../Libraries.hpp"

#include <dlfcn.h>

struct UtlString;

void KeyValues3::load_kv3(const char* text, const KV3ID& kv3_id)
{
	static auto fptr = [] {
		auto* symbol = Libraries::tier0->get_symbol<void* (*)(KeyValues3*, UtlString*, char const*, KV3ID const*, char const*)>("_Z7LoadKV3P10KeyValues3P10CUtlStringPKcRK7KV3ID_tS4_");

		if (symbol)
			Logging::info("Found LoadKV3 at {}", reinterpret_cast<void*>(symbol));
		else
			Logging::error("Couldn't find LoadKV3!");

		return symbol;
	}();

	fptr(this, nullptr, text, &kv3_id, "");
}
