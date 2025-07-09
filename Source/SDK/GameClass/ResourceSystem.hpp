#pragma once

#include "../VirtualMethod.hpp"

#include "../CUtl/BufferString.hpp"

#include <cstdint>

class ResourceSystem {
public:
	VIRTUAL_METHOD(2, query_interface, void*, (const char* name), (this, name));

	VIRTUAL_METHOD(47, load_resource, void*, (BufferString * string, std::uint64_t unk1), (this, string, unk1));

	void precache(const char* resource_path)
	{
		BufferString str = BufferString::just(resource_path);
		str.length = str.length & ~0x3fffffff; // sure...
		load_resource(&str, 0);
	}
};
