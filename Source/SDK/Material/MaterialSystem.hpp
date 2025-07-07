#pragma once

#include <cstdint>

struct Material;
struct KeyValues3;

class MaterialSystem {
public:
	void create_material(Material*** out_material, const char* material_name, KeyValues3* key_values, std::uint32_t unk1, std::uint8_t unk2);

	Material* create_material(const char* name, const char* kv_text_buffer);
};
