#include "CSPlayerPawn.hpp"

#include "Services/CSPlayerItemServices.hpp"

#include "../EngineTrace/GameTrace.hpp"

bool CSPlayerPawn::is_armored_at(HitGroup hit_group) const
{
	if (armor_value() <= 0)
		return false;

	switch (hit_group) {
	case HitGroup::GENERIC:
	case HitGroup::CHEST:
	case HitGroup::STOMACH:
	case HitGroup::LEFTARM:
	case HitGroup::RIGHTARM:
		return true;
		break;
	case HitGroup::HEAD:
		if (auto* item_services = this->item_services();
			item_services && static_cast<CSPlayerItemServices*>(item_services)->has_helmet())
			return true;
		break;
	default:
		break;
	}

	return false;
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
bool CSPlayerPawn::has_heavy_armor() const
{
	// NOTE: Heavy armor was removed from the game by this update:
	// 	https://github.com/SteamDatabase/GameTracking-CS2/commit/a77801f5a6ba82048d235f20e49af027268aabc8
	// 	This was the first out of 2 updates released on July 2nd 2025.
	// I keep this function here, and return a constant value for compatibility reasons and because I believe they might bring it back.
	return false;
}

float CSPlayerPawn::scale_damage_with_armor(float damage, float weapon_armor_ratio, HitGroup hit_group, bool* lost_armor) const
{
	if (lost_armor)
		*lost_armor = false;

	// @see CCSPlayer::OnTakeDamage

	float armor_bonus = 0.5F;
	float armor_ratio = weapon_armor_ratio * 0.5F; // TODO correct?

	float damage_to_health = damage;
	float damage_to_armor = 0.0F;
	float heavy_armor_bonus = 1.0F;

	if (has_heavy_armor()) {
		// TODO: Those values have probably changed
		// 		I took them from W1lliam1337s AutoWall post, but I haven't verified them.
		// 		https://www.unknowncheats.me/forum/counter-strike-2-a/608159-cs2-autowall-penetration-system.html
		armor_ratio *= 0.2F;
		armor_bonus = 0.33F;
		heavy_armor_bonus = 0.25F;
	}

	const bool damage_type_applies_to_armor = true; // Don't even bother calculating this
	int armor_value = this->armor_value();
	if (damage_type_applies_to_armor && armor_value && is_armored_at(hit_group)) {
		damage_to_health = damage * armor_ratio;
		damage_to_armor = (damage - damage_to_health) * (armor_bonus * heavy_armor_bonus);

		if (damage_to_armor > static_cast<float>(armor_value)) {
			damage_to_health = damage - static_cast<float>(armor_value) / armor_bonus;
			damage_to_armor = static_cast<float>(armor_value);
			armor_value = 0;
		} else {

			if (damage_to_armor < 0.0F)
				damage_to_armor = 1.0F;

			armor_value -= static_cast<int>(damage_to_armor);
		}

		damage = damage_to_health;

		if (armor_value <= 0) {
			// Maybe the caller can make tactical decisions based on this?
			if (lost_armor)
				*lost_armor = true;
		}
	}

	return damage;
}
