#pragma once

#include "magic_enum/magic_enum.hpp"

#include <array>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <string_view>

// @schema GrenadeType_t
// NOLINTNEXTLINE(performance-enum-size)
enum GrenadeType : std::uint32_t {
	GRENADE_TYPE_EXPLOSIVE = 0,
	GRENADE_TYPE_FLASH = 1,
	GRENADE_TYPE_FIRE = 2,
	GRENADE_TYPE_DECOY = 3,
	GRENADE_TYPE_SMOKE = 4,
	GRENADE_TYPE_SENSOR = 5,
	GRENADE_TYPE_SNOWBALL = 6,
	GRENADE_TYPE_TOTAL = 7,
};

static constexpr std::array<std::string_view, magic_enum::enum_count<GrenadeType>()> GRENADE_NAMES{
	// GRENADE_TYPE_EXPLOSIVE
	"weapon_hegrenade",
	// GRENADE_TYPE_FLASH
	"weapon_flashbang",
	// GRENADE_TYPE_FIRE
	"weapon_molotov",
	// GRENADE_TYPE_DECOY
	"weapon_decoy",
	// GRENADE_TYPE_SMOKE
	"weapon_smokegrenade",
	// GRENADE_TYPE_SENSOR
	"weapon_tagrenade",
	// GRENADE_TYPE_SNOWBALL
	"weapon_snowball",
};

template <typename BasicJsonType>
inline void to_json(BasicJsonType& j, const GrenadeType& e)
{
	j = GRENADE_NAMES[e];
}

template <typename BasicJsonType>
inline void from_json(const BasicJsonType& j, GrenadeType& e)
{
	const auto entries = magic_enum::enum_values<GrenadeType>();
	std::string s = j.template get<std::string>();
	for (const auto& grenade_type : entries) {
		if (GRENADE_NAMES[grenade_type] == s) {
			e = grenade_type;
			return;
		}
	}
	throw std::invalid_argument{ j.dump() };
}
