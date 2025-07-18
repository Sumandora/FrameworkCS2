#pragma once

#include <cstdint>

// @schema LifeState_t
// NOLINTNEXTLINE(performance-enum-size)
enum LifeState : std::uint8_t {
	LIFE_ALIVE = 0,
	LIFE_DYING = 1,
	LIFE_DEAD = 2,
	LIFE_RESPAWNABLE = 3,
	LIFE_RESPAWNING = 4,
};
