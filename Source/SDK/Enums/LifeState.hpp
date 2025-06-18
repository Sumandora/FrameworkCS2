#pragma once

#include <cstdint>

enum class LifeState : std::uint8_t {
	ALIVE,
	DYING,
	DEAD,
	RESPAWNABLE,
	DISCARDBODY
};
