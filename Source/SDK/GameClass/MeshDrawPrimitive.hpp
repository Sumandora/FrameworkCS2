#pragma once

#include "../Padding.hpp"

#include "../EntityHandle.hpp"

#include <cstdint>

struct BaseEntity;
struct Material;

struct SceneObject { }; // TODO Don't know who owns what really...

struct SceneAnimatableObject : public SceneObject {
	PADDING(0xA8); // Determined through bruteforce, unfortunately
	EntityHandle<BaseEntity> owner;
};

struct ParticleSystemDefinition {
	void* vtable;
	char** name;
};

struct ParticleCollection {
	void* vtable;
	PADDING(0x10);
	ParticleSystemDefinition* particle_system_definition;
};

struct MeshDrawPrimitive {
	ParticleCollection* particle_collection; // TODO Probably specific to particles draw array...
	PADDING(0x10);
	SceneAnimatableObject* scene_animatable_object; // TODO This changes based on the DrawArray function
	Material* material;
	PADDING(0x10);
	PADDING(0x18);
	std::uint32_t color;
	PADDING(0x14);
};
static_assert(sizeof(MeshDrawPrimitive) == 0x68);
