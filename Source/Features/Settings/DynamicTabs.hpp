#pragma once

#include "../Setting.hpp"

#include "MetaSetting.hpp"

#include "imgui.h"

#include "../../GUI/Elements/BoxedTabItem.hpp"

#include <cstddef>
#include <ranges>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

template <typename T>
	requires std::is_base_of_v<MetaSetting, T>
class DynamicTabs : public Setting, SettingsHolder {
	std::string child_name;

public:
	explicit DynamicTabs(SettingsHolder* parent, std::string name, std::string child_name, bool default_element = true)
		: Setting(parent, name)
		, child_name(std::move(child_name))
	{
		if (default_element)
			new T{ this, this->child_name };
	}

	auto get() const
	{
		return settings | std::ranges::views::transform([](Setting* setting) {
			return static_cast<T*>(setting);
		});
	}

	void render() override
	{
		if (ImGui::BeginTabBar(get_name().c_str(), ImGuiTabBarFlags_None)) {
			for (std::size_t i = 0; i < settings.size();) {
				auto it = settings.begin() + i;
				Setting* setting = *it;
				bool open = true;
				// TODO add before/after buttons?
				if (ImGuiExt::BeginBoxedTabItem((setting->get_name() + " " + std::to_string(i)).c_str(), {}, &open)) {
					setting->render();
					ImGuiExt::EndBoxedTabItem();
				}

				if (!open)
					settings.erase(settings.begin() + i);
				else
					i++;
			}

			if (ImGui::TabItemButton("+", ImGuiTabItemFlags_Trailing | ImGuiTabItemFlags_NoTooltip)) {
				new T{ this, child_name };
			}

			ImGui::EndTabBar();
		}
	}

	// Because childs have duplicate names, they are indexed by number.
	void serialize(nlohmann::json& output_json) const override
	{
		for (std::size_t i = 0; i < settings.size(); i++)
			settings.at(i)->serialize(output_json[i]);
	}

	void deserialize(const nlohmann::json& input_json) override
	{
		if (settings.size() != input_json.size()) {
			if (input_json.size() < settings.size())
				settings.erase(settings.begin() + input_json.size(), settings.begin() + settings.size());
			else
				for (std::size_t left = 0; left < input_json.size() - settings.size(); left++)
					new T{ this, child_name };
		}

		for (std::size_t i = 0; i < input_json.size(); i++)
			settings.at(i)->deserialize(input_json[i]);
	}
};
