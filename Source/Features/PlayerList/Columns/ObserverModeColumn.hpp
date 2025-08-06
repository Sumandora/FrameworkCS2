#pragma once

#include "../Columns.hpp"

#include "../../../SDK/Entities/BasePlayerPawn.hpp"
#include "../../../SDK/Entities/CSPlayerController.hpp"
#include "../../../SDK/Entities/CSPlayerPawnBase.hpp"
#include "../../../SDK/Entities/Services/PlayerObserverServices.hpp"
#include "../../../SDK/EntityHandle.hpp"

#include "../../../Utils/Logging.hpp"

#include "imgui.h"

#include "magic_enum/magic_enum.hpp"

#include <format>
#include <string>
#include <string_view>
#include <utility>

struct ObserverModeColumn {
	static constexpr const char* NAME = "Observer Mode";
	std::string observer_mode_name;
	ObserverMode last_mode = NUM_OBSERVER_MODES;
	bool enabled = false;

	void update(CSPlayerController* controller)
	{
		if (!enabled) {
			observer_mode_name.clear();
			last_mode = NUM_OBSERVER_MODES;
			return;
		}

		const BasePlayerPawn* pawn = controller->pawn().get();
		if (!pawn) {
			observer_mode_name.clear();
			last_mode = NUM_OBSERVER_MODES;
			return;
		}

		const PlayerObserverServices* observer_services = pawn->observer_services();
		if (!observer_services) {
			observer_mode_name.clear();
			last_mode = NUM_OBSERVER_MODES;
			return;
		}

		if (!observer_services->observer_target().has_entity()) {
			observer_mode_name.clear();
			last_mode = NUM_OBSERVER_MODES;
			return;
		}

		const ObserverMode mode = observer_services->observer_mode();

		if (last_mode == mode) {
			return; // Work done...
		}

		if (mode < 0 || mode >= NUM_OBSERVER_MODES) {
			const std::string_view enum_name = magic_enum::enum_name(mode);
			Logging::error("Unknown observer mode {} -> {}", std::to_underlying(mode), enum_name);
			observer_mode_name = enum_name;
		} else
			observer_mode_name = OBSERVER_MODE_NAMES.at(mode);
		last_mode = mode;
	}

	void fill() const
	{
		if (!observer_mode_name.empty())
			ImGui::Text("%s", observer_mode_name.c_str());
	}
};

static_assert(Column<ObserverModeColumn>);
