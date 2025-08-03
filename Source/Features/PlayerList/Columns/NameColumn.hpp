#pragma once

#include "../Columns.hpp"

#include "../../../SDK/Entities/CSPlayerController.hpp"

#include "imgui.h"

#include <optional>
#include <string>

struct NameColumn {
	static constexpr const char* NAME = "Name";
	std::optional<std::string> cached_name;

	void update(CSPlayerController* controller)
	{
		if (!cached_name.has_value()) {
			cached_name = controller->get_decorated_player_name();
		}
	}

	void fill() const
	{
		const char* name
			= cached_name
				  .transform([](const std::string& s) {
					  return s.c_str();
				  })
				  .value_or("TBD");
		ImGui::Text("%s", name);
	}
};

static_assert(Column<NameColumn>);
