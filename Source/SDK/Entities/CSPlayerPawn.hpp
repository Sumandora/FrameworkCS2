#pragma once

#include "../Schema/FieldOffset.hpp"

#include "../CUtl/Vector.hpp"

#include "CSPlayerPawnBase.hpp"

#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"

#include "../EngineTrace/GameTrace.hpp"

#include <cstdint>

struct CSPlayerPawn : public CSPlayerPawnBase {
	CLASS_INFO("libclient.so", "C_CSPlayerPawn");

	SCHEMA_VAR(bool, isScoped, "m_bIsScoped");

	SCHEMA_VAR(int, shots_fired, "m_iShotsFired");

	SCHEMA_VAR(UtlVector<glm::vec3>, aim_punch_cache, "m_aimPunchCache"); // TODO don't use this

	SCHEMA_VAR(std::int32_t, armor_value, "m_ArmorValue");

	[[nodiscard]] bool is_armored_at(HitGroup hit_group) const;
	[[nodiscard]] bool has_heavy_armor() const;

	[[nodiscard]] float scale_damage_with_armor(
		float unscaled_damage,
		float weapon_armor_ratio,
		HitGroup hit_group,
		bool* lost_armor = nullptr) const;

	[[nodiscard]] bool can_perform_primary_attack() const;
	[[nodiscard]] bool can_perform_secondary_attack() const;

	[[nodiscard]] glm::vec2 get_aim_punch(float fraction = 0.0F);
};
