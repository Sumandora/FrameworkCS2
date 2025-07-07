#pragma once

#include "../Padding.hpp"
#include <cstdint>

struct Material;

struct MeshDrawPrimitive {
	PADDING(0x20);
	Material* material;
	PADDING(0x28);
	std::uint32_t color;
	PADDING(0x14);
};
static_assert(sizeof(MeshDrawPrimitive) == 0x68);
