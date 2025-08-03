#pragma once

#include "../Columns.hpp"

#include "../../../SDK/Entities/CSPlayerPawn.hpp"
#include "../../../SDK/Enums/TeamID.hpp"
#include "../../../SDK/GameClass/Localize.hpp"

#include "../../../Interfaces.hpp"

#include "imgui.h"

#include <optional>
#include <string>

struct TeamColumn {
	static constexpr const char* NAME = "Team";
	std::optional<std::string> localization;
	TeamID last_teamid;

	void update(CSPlayerPawn* pawn)
	{
		if (!pawn) {
			localization.reset();
			return;
		}
		if (localization.has_value() && last_teamid == pawn->team_id())
			return; // Work was already done.
		last_teamid = pawn->team_id();
		localization = Interfaces::localize->translate(get_localization_key(pawn->team_id()));
	}

	void fill() const
	{
		if (localization.has_value()) {
			ImGui::Text("%s", localization->c_str());
		}
	}
};

static_assert(Column<TeamColumn>);
