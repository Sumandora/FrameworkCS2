#include "CSPlayerPawn.hpp"

#include "../EngineTrace/GameTrace.hpp"
#include "../GameClass/GlobalVars.hpp"
#include "BasePlayerWeapon.hpp"
#include "CSWeaponBase.hpp"
#include "Services/CSPlayerItemServices.hpp"
#include "Services/CSPlayerWeaponServices.hpp"
#include "Services/PlayerWeaponServices.hpp"
#include "WeaponRevolver.hpp"

#include "BCRL/SearchConstraints.hpp"
#include "BCRL/Session.hpp"
#include "SignatureScanner/PatternSignature.hpp"

#include "../../Memory.hpp"

#include "glm/ext/vector_float2.hpp"

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
			item_services && item_services->has_helmet())
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

// TODO This shouldn't be a member of CSPlayerPawn, because weapon_services and weapon should be acquired by the caller, but whatever...
bool CSPlayerPawn::can_perform_primary_attack() const
{
	if (wait_for_no_attack())
		return false;

	const CSPlayerWeaponServices* weapon_services = this->weapon_services();
	if (!weapon_services)
		return false;

	BasePlayerWeapon* weapon = weapon_services->active_weapon().get();
	if (!weapon)
		return false;

	if (weapon->clip1() <= 0)
		return false;

	const auto* cs_weapon = weapon->entity_cast<CSWeaponBase*>();
	if (cs_weapon && cs_weapon->in_reload())
		return false;

	const float curtime = (*Memory::globals)->current_time();

	if (weapon_services->next_attack() > curtime)
		return false;

	if (weapon->next_primary_attack_tick().as_time() > curtime)
		return false;

	if (cs_weapon
		&& weapon->getSchemaType() == WeaponRevolver::classInfo()
		&& cs_weapon->postpone_fire_ready_ticks().as_time() > curtime)
		return false;

	return true;
}

bool CSPlayerPawn::can_perform_secondary_attack() const
{
	// TODO
	return true;
}

static glm::vec2 (*get_aim_punch)(CSPlayerPawn*, float tick_fraction, char unk) = nullptr;

void CSPlayerPawn::resolve_signatures()
{
	// TODO better signature (probably hard)
	::get_aim_punch
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_array_of_bytes<"55 48 89 E5 41 57 66 41 0F 7E C7 41 56 4C 8D 75 ? 41 55 4C 8D 6D ? 41 54 49 89 FC">(),
			BCRL::everything(Memory::mem_mgr).with_flags("r-x").with_name("libclient.so"))
			  .BCRL_EXPECT(decltype(::get_aim_punch), ::get_aim_punch);
}

glm::vec2 CSPlayerPawn::get_aim_punch(float fraction)
{
	return ::get_aim_punch(this, fraction, 1);
}
