#pragma once

#include "../VirtualMethod.hpp"

#include "../CUtl/BufferString.hpp"

#include <cstdint>

class ResourceSystem {
public:
	VIRTUAL_METHOD(45, load_resource, void*, (BufferString * string, std::uint64_t unk1), (this, string, unk1));
	VIRTUAL_METHOD(87, delete_resource, void, (void* resource), (this, resource));

	void precache(const char* resource_path)
	{
		BufferString str = BufferString::just(resource_path);
		auto prev_length = str.length;
		str.length = str.length & ~0x3fffffff; // sure...
		load_resource(&str, 0);
		str.length = prev_length;
	}
};
