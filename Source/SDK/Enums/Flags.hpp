#pragma once

#include <cstdint>

// @schema Flags_t
enum Flags : std::uint32_t {
	FL_ONGROUND = 1U << 0,
	FL_DUCKING = 1U << 1,
	FL_WATERJUMP = 1U << 2,
	FL_BOT = 1U << 4,
	FL_FROZEN = 1U << 5,
	FL_ATCONTROLS = 1U << 6,
	FL_CLIENT = 1U << 7,
	FL_FAKECLIENT = 1U << 8,
	FL_FLY = 1U << 10,
	FL_SUPPRESS_SAVE = 1U << 11,
	FL_IN_VEHICLE = 1U << 12,
	FL_GODMODE = 1U << 14,
	FL_NOTARGET = 1U << 15,
	FL_AIMTARGET = 1U << 16,
	FL_STATICPROP = 1U << 18,
	FL_GRENADE = 1U << 20,
	FL_DONTTOUCH = 1U << 22,
	FL_BASEVELOCITY = 1U << 23,
	FL_CONVEYOR = 1U << 24,
	FL_OBJECT = 1U << 25,
	FL_ONFIRE = 1U << 27,
	FL_DISSOLVING = 1U << 28,
	FL_TRANSRAGDOLL = 1U << 29,
	FL_UNBLOCKABLE_BY_PLAYER = 1U << 30,
};
