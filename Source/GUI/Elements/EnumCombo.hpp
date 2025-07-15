#pragma once

#include "imgui.h"
#include "magic_enum/magic_enum.hpp"

#include <algorithm>
#include <string>
#include <string_view>
#include <type_traits>

template <typename E>
	requires std::is_enum_v<E>
struct EnumNames {
	// Gives a (value,name) map
	static constexpr auto NAMES = magic_enum::enum_entries<E>();
};

namespace ImGuiExt {
	template <typename E>
		requires std::is_enum_v<E>
	// NOLINTNEXTLINE(readability-identifier-naming) Mirrors ImGui naming convention
	bool EnumCombo(const char* label, E& value)
	{
		bool changed = false;
		std::string current_name{ (*std::ranges::find(EnumNames<E>::NAMES, value, [](const auto& pair) { return pair.first; })).second };
		if (ImGui::BeginCombo(label, current_name.c_str(), ImGuiComboFlags_None)) {
			for (const auto& [other_value, name] : EnumNames<E>::NAMES) {
				bool selected = other_value == value;
				if (ImGui::Selectable(std::string{ name }.c_str(), selected)) {
					value = other_value;
					changed = true;
				}

				if (selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		return changed;
	}
}
