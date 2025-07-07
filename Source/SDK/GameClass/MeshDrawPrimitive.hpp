#pragma once

#include "../Padding.hpp"

#include <cstdint>

#include "../EntityHandle.hpp"

struct BaseEntity;
struct Material;

struct SceneAnimatableObject {
	PADDING(0xA8); // Determined through bruteforce, unfortunately
	EntityHandle<BaseEntity> owner;
};

struct MeshDrawPrimitive {
	PADDING(0x18);
	SceneAnimatableObject* scene_animatable_object;
	Material* material;
	PADDING(0x10);
	PADDING(0x18);
	std::uint32_t color;
	PADDING(0x14);
};
static_assert(sizeof(MeshDrawPrimitive) == 0x68);
