#pragma once

#include "../Padding.hpp"

#include <cstdint>

struct UtlBuffer;

struct KV3ID {
	const char* name;
	std::uint64_t magic1;
	std::uint64_t magic2;
};

struct KeyValues3 {
	PADDING(1024 * 10); // TODO figure out correct size

	static void resolve_functions();
	void load_kv3(const char* text, const KV3ID& kv3_id);
};
