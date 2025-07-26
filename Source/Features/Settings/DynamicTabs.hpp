#pragma once

#include "../Setting.hpp"

#include "MetaSetting.hpp"

#include "../../GUI/Elements/BoxedTabItem.hpp"

#include "../../Utils/Logging.hpp"

#include "imgui.h"
#include "imgui_internal.h"

#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <mutex>
#include <ranges>
#include <shared_mutex>
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

	mutable std::shared_mutex mutex;

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

	std::shared_lock<std::shared_mutex> read_lock() const
	{
		return std::shared_lock{ mutex };
	}

	std::unique_lock<std::shared_mutex> write_lock() const
	{
		return std::unique_lock{ mutex };
	}

	// Call read_lock before calling this.
	auto get() const
	{
		return settings | std::ranges::views::transform([](Setting* setting) {
			return static_cast<T*>(setting);
		});
	}

	void render() override
	{
		// There is no lock assigned here except for the write_lock when adding or removing an element
		// This works because the settings itself have a constant size, so you at worst read old data.
		// However when the settings vector itself changes, then reading old data may result in SIGSEGV.

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

				if (!open) {
					const std::unique_lock scoped_lock = write_lock();

					settings.erase(it);
				} else
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
				const std::unique_lock scoped_lock = write_lock();

				auto* new_element = new IndexedT{ this, child_name };
				new_element->id = id_counter++;
			}

			ImGui::EndTabBar();
		}
	}

	// Because childs have duplicate names, they are indexed by number.
	void serialize(nlohmann::json& output_json) const override
	{
		const std::unique_lock scoped_lock = write_lock();

		for (std::size_t i = 0; i < settings.size(); i++)
			settings.at(i)->serialize(output_json[i]);
	}

	void deserialize(const nlohmann::json& input_json) override
	{
		const std::unique_lock scoped_lock = write_lock();

		if (settings.size() != input_json.size()) {
			if (input_json.size() < settings.size()) {
				settings.erase(settings.begin() + input_json.size(), settings.begin() + settings.size());
			} else {
				std::size_t remaining = input_json.size() - settings.size(); // Evaluate before going into the for loop, because settings.size() will change.
				for (std::size_t left = 0; left < remaining; left++) {
					auto* new_element = new IndexedT{ this, child_name };
					new_element->id = id_counter++;
				}
			}
		}

		for (std::size_t i = 0; i < input_json.size(); i++)
			settings.at(i)->deserialize(input_json[i]);
	}
};
