#pragma once

#include "../Setting.hpp"

#include "MetaSetting.hpp"

#include "imgui.h"

#include "../../GUI/Elements/BoxedTabItem.hpp"
#include "imgui_internal.h"

#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <ranges>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

template <typename T>
	requires std::is_base_of_v<MetaSetting, T>
class DynamicTabs : public Setting, SettingsHolder {
	std::string child_name;

	struct IndexedT : public T {
		using T::T;
		std::size_t id;
	};

	std::size_t id_counter = 1;

public:
	explicit DynamicTabs(SettingsHolder* parent, std::string name, std::string child_name, bool default_element = true)
		: Setting(parent, name)
		, child_name(std::move(child_name))
	{
		if (default_element) {
			auto* new_element = new IndexedT{ this, this->child_name };
			new_element->id = id_counter++;
		}
	}

	auto get() const
	{
		return settings | std::ranges::views::transform([](Setting* setting) {
			return static_cast<T*>(setting);
		});
	}

	void render() override
	{
		if (ImGui::BeginTabBar(get_name().c_str(), ImGuiTabBarFlags_Reorderable)) {
			ImGuiTabBar* tab_bar = ImGui::GetCurrentTabBar();
			ImGuiTabItem* tab_item = nullptr;
			std::size_t selected = 0;

			for (std::size_t i = 0; i < settings.size();) {
				auto it = settings.begin() + i;
				auto* setting = static_cast<IndexedT*>(*it);
				bool open = true;

				if (ImGuiExt::BeginBoxedTabItem((setting->get_name() + " " + std::to_string(setting->id)).c_str(), {}, &open)) {
					selected = i;
					tab_item = ImGui::TabBarGetCurrentTab(tab_bar);
					setting->render();
					ImGuiExt::EndBoxedTabItem();
				}

				if (!open)
					settings.erase(it);
				else
					i++;
			}

			// HACK
			if (tab_bar->ReorderRequestTabId != 0) {
				const std::size_t order = ImGui::TabBarGetTabOrder(tab_bar, tab_item) + tab_bar->ReorderRequestOffset;
				if (selected != order) {
					std::iter_swap(settings.begin() + selected, settings.begin() + order);
				}
			}

			if (ImGui::TabItemButton("+", ImGuiTabItemFlags_Trailing | ImGuiTabItemFlags_NoTooltip)) {
				auto* new_element = new IndexedT{ this, child_name };
				new_element->id = id_counter++;
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
			if (input_json.size() < settings.size()) {
				settings.erase(settings.begin() + input_json.size(), settings.begin() + settings.size());
				id_counter = settings.size() + 1;
			} else {
				id_counter = 1;
				for (std::size_t left = 0; left < input_json.size() - settings.size(); left++) {
					auto* new_element = new IndexedT{ this, child_name };
					new_element->id = id_counter++;
				}
			}
		}

		for (std::size_t i = 0; i < input_json.size(); i++)
			settings.at(i)->deserialize(input_json[i]);
	}
};
