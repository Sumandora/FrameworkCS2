#pragma once

#include <cstdint>

#include "../VirtualMethod.hpp"

struct Material;
struct KeyValues3;

class MaterialSystem {
public:
	STATIC_VIRTUAL_METHOD(28, create_material, void,
	(Material*** out_material, const char* material_name, KeyValues3* key_values, std::uint32_t unk1, std::uint8_t unk2),
	(this, out_material, nullptr, material_name, key_values, unk1, unk2));

	Material** create_material(const char* name, const char* kv_text_buffer);
};
