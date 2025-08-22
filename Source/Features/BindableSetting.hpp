#pragma once

#include "Setting.hpp"
#include "Settings/MetaSetting.hpp"

#include "../GUI/Elements/ResizablePopup.hpp"
#include "../GUI/GUI.hpp"
#include "../GUI/InputManager.hpp"

#include "imgui.h"
#include "imgui_internal.h"

#include "nlohmann/json.hpp"

#include <cstddef>
#include <functional>
#include <iterator>
#include <memory>
#include <mutex>
#include <ranges>
#include <shared_mutex>
#include <string>
#include <type_traits>
#include <utility>

template <typename T>
struct SettingWithBind : public T {
	using T::T;
	std::shared_ptr<GUI::InputManager::Key> key = nullptr;
};

/**
 * Architectural notes:
 * 	Each BindableSetting is a MetaSetting where
 * 		- the first setting is the original setting
 * 		- the rest settings are SettingWithBind<T> which are a copy of the original setting (with different value of course) and a key
 */

template <typename T>
	requires std::is_base_of_v<Setting, T>
class BindableSetting : public MetaSetting {
	mutable std::shared_mutex mutex;
	T original_setting; // Store the original setting here, so we don't do an unnecessary heap allocation

	// Used to create new settings later (NOTE: Don't store this pointer, because this could be moved)
	std::function<SettingWithBind<T>*(BindableSetting* self)> factory;

	using Result = decltype(std::declval<T&>().get());

public:
	template <typename... Args>
	explicit BindableSetting(SettingsHolder* parent, std::string name, Args... args)
		: MetaSetting(parent, name)
		, original_setting(this, name, std::forward<Args>(args)...)
		, factory([args...](BindableSetting* self) { return new SettingWithBind<T>{ self, self->get_name(), args... }; })
	{
	}

	// Provided mainly for visiblity conditions, which shouldn't change based on game state
	Result get_raw() const
	{
		return original_setting.get();
	}

	Result get() const
	{
		const std::shared_lock lock{ mutex };
		for (const Setting* setting : settings | std::ranges::views::drop(1)) {
			const SettingWithBind<T>& setting_with_bind = *static_cast<const SettingWithBind<T>*>(setting);
			if (setting_with_bind.key->is_active()) {
				return setting_with_bind.get();
			}
		}
		return get_raw();
	}

	template <typename... Args>
	auto set(Args&&... args)
	{
		original_setting.set(std::forward<Args>(args)...);
	}

	void render() override
	{
		original_setting.render();

		ImGui::PushID(this);
		ImGui::OpenPopupOnItemClick("Binds");

		if (ImGuiExt::BeginResizablePopup("Binds", ImGuiWindowFlags_AlwaysAutoResize)) {
			// This lock is being held for an extended amount of time here, however it is only while rendering.
			// `get` will be called while in game, so they shouldn't intersect that much.
			const std::unique_lock lock{ mutex };

			auto erase_iterator = settings.end();

			auto it = std::next(settings.begin());
			while (it != settings.end()) {
				ImGui::PushID(*it);

				SettingWithBind<T>& setting_with_bind = *static_cast<SettingWithBind<T>*>(*it);

				ImGui::GetCurrentWindow()->DC.CurrLineTextBaseOffset = ImGui::GetStyle().FramePadding.y;
				ImGui::TextUnformatted("Activate");

				ImGui::SameLine();
				if (ImGui::Button("state"))
					ImGui::OpenPopup("##State");

				if (ImGui::BeginPopup("##State")) {
					setting_with_bind.render();
					ImGui::EndPopup();
				}

				ImGui::SameLine();
				ImGui::TextUnformatted("when");

				ImGui::SameLine();
				setting_with_bind.key->draw_bind();

				ImGui::SameLine();
				ImGui::TextUnformatted("is");

				ImGui::SameLine();
				setting_with_bind.key->draw_type();

				static constexpr float DELETE_SPACING = 30.0F;
				ImGui::SameLine(0.0F, DELETE_SPACING * GUI::get_scale());
				if (ImGui::Button("Remove"))
					erase_iterator = it;

				ImGui::PopID();

				it++;
			}

			// Erase after the loop ended to avoid some ugly code
			if (erase_iterator != settings.end())
				settings.erase(erase_iterator);

			if (ImGui::Button("New keybind")) {
				SettingWithBind<T>* new_bind = factory(this);
				new_bind->key = std::make_shared<GUI::InputManager::Key>();
				GUI::get_input_manager().register_key(new_bind->key);
			}

			ImGui::EndPopup();
		}
		ImGui::PopID();
	}

	void serialize(nlohmann::json& output_json) const override
	{
		original_setting.serialize(output_json["Main"]);

		if (settings.size() == 1 /* Only the original setting */)
			return;

		auto& binds_array = output_json["Binds"];
		const std::unique_lock lock{ mutex };
		for (const Setting* setting : settings | std::ranges::views::drop(1)) {
			const SettingWithBind<T>& setting_with_bind = *static_cast<const SettingWithBind<T>*>(setting);
			nlohmann::json json_bind{
				{ "Key", *setting_with_bind.key },
			};
			setting_with_bind.serialize(json_bind["Value"]);
			binds_array.push_back(json_bind);
		}
	}

	void deserialize(const nlohmann::json& input_json) override
	{
		original_setting.deserialize(input_json["Main"]);
		if (input_json.contains("Binds")) {
			const std::unique_lock lock{ mutex };

			const auto& binds_array = input_json["Binds"];

			static constexpr std::size_t ORIGINAL_SETTING = 1; // Because the first setting is the main one, there is some dodgy math required here.

			if (binds_array.size() < settings.size() - ORIGINAL_SETTING) {
				settings.erase(settings.begin() + ORIGINAL_SETTING + binds_array.size(), settings.begin() + settings.size());
			} else if (binds_array.size() > settings.size() - ORIGINAL_SETTING) {
				std::size_t remaining = binds_array.size() - (settings.size() - ORIGINAL_SETTING);
				for (std::size_t left = 0; left < remaining; left++) {
					factory(this);
				}
			}

			for (std::size_t i = 0; i < binds_array.size(); i++) {
				const auto& json_bind = binds_array[i];
				SettingWithBind<T>& setting_with_bind = *static_cast<SettingWithBind<T>*>(std::to_address(settings[ORIGINAL_SETTING + i]));
				setting_with_bind.deserialize(json_bind["Value"]);
				setting_with_bind.key = std::make_shared<GUI::InputManager::Key>(json_bind.value("Key", GUI::InputManager::Key{}));
				GUI::get_input_manager().register_key(setting_with_bind.key);
			}
		}
	}
};
