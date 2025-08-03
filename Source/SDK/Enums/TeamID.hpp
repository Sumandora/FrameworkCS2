#pragma once

// NOLINTNEXTLINE(performance-enum-size)
enum class TeamID : int {
	TEAM_INVALID = -1,
	TEAM_ANY = -1,
	TEAM_UNASSIGNED = 0,
	TEAM_SPECTATOR = 1,
	TEAM_TERRORIST = 2,
	TEAM_COUNTER_TERRORIST = 3,
};

