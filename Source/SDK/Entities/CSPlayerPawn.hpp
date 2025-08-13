#pragma once

#include "../Schema/FieldOffset.hpp"

#include "CSPlayerPawnBase.hpp"

#include "glm/ext/vector_float2.hpp"

#include "../EngineTrace/GameTrace.hpp"

#include <cstdint>

struct CSPlayerPawn : public CSPlayerPawnBase {
	CLASS_INFO("libclient.so", "C_CSPlayerPawn");

	SCHEMA_VAR(bool, is_scoped, "m_bIsScoped");

	SCHEMA_VAR(int, shots_fired, "m_iShotsFired");

	SCHEMA_VAR(std::int32_t, armor_value, "m_ArmorValue");

	SCHEMA_VAR(bool, in_buy_zone, "m_bInBuyZone");

	SCHEMA_VAR(bool, wait_for_no_attack, "m_bWaitForNoAttack");

	[[nodiscard]] bool is_armored_at(HitGroup hit_group) const;
	[[nodiscard]] bool has_heavy_armor() const;

	[[nodiscard]] float scale_damage_with_armor(
		float unscaled_damage,
		float weapon_armor_ratio,
		HitGroup hit_group,
		bool* lost_armor = nullptr) const;

	[[nodiscard]] bool can_perform_primary_attack() const;
	[[nodiscard]] bool can_perform_secondary_attack() const;

	static void resolve_signatures();
	[[nodiscard]] glm::vec2 get_aim_punch(float fraction = 0.0F);
};
