#pragma once

#include "../Columns.hpp"

#include "../../../SDK/Entities/CSPlayerPawn.hpp"

#include "imgui.h"

#include <optional>

struct HealthColumn {
	static constexpr const char* NAME = "Health";
	struct Health {
		int value;
		int max;
	};
	std::optional<Health> health;

	void update(CSPlayerPawn* pawn)
	{
		if (!pawn) {
			health.reset();
			return;
		}
		health = { .value = pawn->health(), .max = pawn->max_health() };
	}

	void fill() const
	{
		if (health.has_value()) {
			ImGui::Text("%d/%d", health->value, health->max);
		}
	}
};

static_assert(Column<HealthColumn>);
