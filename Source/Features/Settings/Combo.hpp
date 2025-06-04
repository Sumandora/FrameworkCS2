#pragma once

#include "../Setting.hpp"

#include "imgui.h"

#include "magic_enum/magic_enum.hpp"

#include <string>
#include <unordered_map>
#include <utility>

template <typename E>
	requires std::is_enum_v<E>
class Combo : public Setting {
	std::unordered_map<E, std::string> names;
	E value;

	static std::unordered_map<E, std::string> generate_names()
	{
		std::unordered_map<E, std::string> names;
		// TODO case conversion
		for (auto [value, name] : magic_enum::enum_entries<E>()) {
			names[value] = name;
		}
		return names;
	}

public:
	Combo(SettingsHolder* parent, std::string name, E value = magic_enum::enum_values<E>()[0], std::unordered_map<E, std::string> names = generate_names())
		: Setting(parent, std::move(name))
		, names(std::move(names))
		, value(value)
	{
	}

	[[nodiscard]] E get() const { return value; }

	void render() override
	{
		// TODO this iteration is slow, but I can't iterate over the map, because its unordered and even std::map does not keep order (ordered_map soonTM)
		if (ImGui::BeginCombo(get_name().c_str(), names[value].c_str(), ImGuiComboFlags_None)) {
			for (E value : magic_enum::enum_values<E>()) {
				bool selected = value == this->value;
				if (ImGui::Selectable(names[value].c_str(), selected))
					this->value = value;

				if (selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
	}
	void serialize(nlohmann::json& output_json) const override
	{
		output_json = std::to_underlying(value);
	}
	void deserialize(const nlohmann::json& input_json) override
	{
		value = static_cast<E>(input_json);
	}
};
