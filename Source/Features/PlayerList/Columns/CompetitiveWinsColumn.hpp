#pragma once

#include "../Columns.hpp"

#include "../../../SDK/Entities/CSPlayerController.hpp"

#include "imgui.h"

struct CompetitiveWinsColumn {
	static constexpr const char* NAME = "Competitive Wins";
	int wins;

	void update(CSPlayerController* controller)
	{
		wins = controller->competitive_wins();
	}

	void fill() const
	{
		ImGui::Text("%d", wins);
	}
};

static_assert(Column<CompetitiveWinsColumn>);
