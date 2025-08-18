#pragma once

#include "../Columns.hpp"

#include "../../../SDK/Entities/CSPlayerController.hpp"

#include "imgui.h"

struct PingColumn {
	static constexpr const char* NAME = "Ping";
	static constexpr bool HIDDEN_BY_DEFAULT = true;
	CSPlayerController::ping_type ping_ms = 0;

	void update(CSPlayerController* controller)
	{
		ping_ms = controller->ping();
	}

	void fill() const
	{
		ImGui::Text("%d", ping_ms);
	}
};

static_assert(Column<PingColumn>);
