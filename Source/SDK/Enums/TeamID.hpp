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

inline const char* get_localization_key(TeamID id)
{
	switch (id) {
	case TeamID::TEAM_TERRORIST:
		return "terrorists";
	case TeamID::TEAM_COUNTER_TERRORIST:
		return "counter-terrorists";
	default:
		return "noteam";
	}
}
