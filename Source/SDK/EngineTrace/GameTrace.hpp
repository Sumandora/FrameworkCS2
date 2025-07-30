#pragma once

#include "../Padding.hpp"

#include "glm/ext/vector_float3.hpp"

#include <cstddef>
#include <cstdint>

struct BaseEntity;

// @schema HitGroup_t
enum class HitGroup : std::uint32_t {
	INVALID = 0xffffffff,
	GENERIC = 0,
	HEAD = 1,
	CHEST = 2,
	STOMACH = 3,
	LEFTARM = 4,
	RIGHTARM = 5,
	LEFTLEG = 6,
	RIGHTLEG = 7,
	NECK = 8,
	UNUSED = 9,
	GEAR = 10,
	SPECIAL = 11,
	COUNT = 12,
};

struct TraceHitboxData {
	PADDING(0x38);
	HitGroup hitgroup;
	PADDING(0x9);
	std::uint32_t hitbox_id;
	PADDING(0x24);
};

struct GameTrace {
	PADDING(8);
	BaseEntity* hit_entity;
	TraceHitboxData* hitbox_data;
	PADDING(95);
	glm::vec3 from;
	glm::vec3 to;
	PADDING(24);
	float fraction;
	PADDING(13);
	PADDING(6969);

	static GameTrace initialized();
};

// static_assert(sizeof(GameTrace) == 0xc0);
