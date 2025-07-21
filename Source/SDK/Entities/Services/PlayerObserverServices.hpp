#pragma once

#include "../../Schema/FieldOffset.hpp"

#include "PlayerPawnComponent.hpp"

#include "../BaseEntity.hpp"

#include <cstdint>

// @schema ObserverMode_t
// NOLINTNEXTLINE(performance-enum-size)
enum ObserverMode : std::uint32_t {
	OBS_MODE_NONE = 0,
	OBS_MODE_FIXED = 1,
	OBS_MODE_IN_EYE = 2,
	OBS_MODE_CHASE = 3,
	OBS_MODE_ROAMING = 4,
	OBS_MODE_DIRECTED = 5,
	NUM_OBSERVER_MODES = 6,
};

struct PlayerObserverServices : public PlayerPawnComponent {
	CLASS_INFO("libclient.so", "CPlayer_ObserverServices");

private:
	SCHEMA_VAR(std::uint8_t, raw_observer_mode, "m_iObserverMode");

public:
	[[nodiscard]] ObserverMode observer_mode() const
	{
		return static_cast<ObserverMode>(raw_observer_mode());
	}

	SCHEMA_VAR(EntityHandle<BaseEntity>, observer_target, "m_hObserverTarget");
};
