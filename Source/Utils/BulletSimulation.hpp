#pragma once

#include "glm/ext/vector_float3.hpp"

#include "../SDK/EngineTrace/GameTrace.hpp"

struct BaseEntity;
struct CSPlayerPawn;

namespace BulletSimulation {
	void resolve_signatures();

	struct Results {
		float raw_damage;
		float scaled_damage;

		HitGroup hit_group;

		bool lost_armor;

		int penetration_count;
		BaseEntity* hit_entity;
	};

	Results simulate_bullet(const glm::vec3& from, const glm::vec3& to, BaseEntity* entity);
}
