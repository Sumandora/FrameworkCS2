#pragma once

#include "../Padding.hpp"

#include "glm/ext/vector_float3.hpp"

#include <cstddef>
#include <cstdint>

struct BaseEntity;

// NOLINTNEXTLINE(performance-enum-size)
enum class HitGroup : std::uint32_t {
	GENERIC = 0,
	HEAD = 1,
	CHEST = 2,
	STOMACH = 3,
	LEFTARM = 4,
	RIGHTARM = 5,
	LEFTLEG = 6,
	RIGHTLEG = 7,
	GEAR = 10,
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
	PADDING(28);
	float fraction;
	PADDING(4);
	PADDING(7);
	bool unk; // I think this indicates if the trace hit something...
	PADDING(1);

	static GameTrace initialized();
};

static_assert(sizeof(GameTrace) == 0xc0);
