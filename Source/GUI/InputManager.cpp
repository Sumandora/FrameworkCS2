#include "InputManager.hpp"

#include "GUI.hpp"

#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"

#include <algorithm>
#include <array>
#include <cfloat>
#include <memory>
#include <ranges>
#include <string>
#include <utility>

bool GUI::InputManager::Key::is_active() const
{
	// Don't count keybinds if someone is in the menu, that would cause issues with text inputs
	if (type == Type::HOLD && GUI::is_menu_open())
		return false;
	return active;
}

void GUI::InputManager::Key::update_state()
{
	// Don't react to keybind in the menu.
	if (GUI::is_menu_open())
		return;

	// No key? No active.
	if (method == Method::KEY && key == ImGuiKey_None) {
		active = false;
		return;
	}

	switch (type) {
	case Type::HOLD: {
		switch (method) {
		case Method::KEY: {
			active = ImGui::IsKeyDown(key);
			break;
		}
		case Method::MOUSE: {
			active = ImGui::IsMouseDown(mouse_button);
			break;
		}
		}
		break;
	}
	case Type::TOGGLE: {
		switch (method) {
		case Method::KEY: {
			if (ImGui::IsKeyPressed(key, false))
				active = !active;
			break;
		}
		case Method::MOUSE: {
			if (ImGui::IsMouseClicked(mouse_button, false))
				active = !active;
			break;
		}
		}
		break;
	}
	}
}

static float calculate_longest_key_name()
{
	float longest = 0.0F;

	for (ImGuiKey imgui_key = ImGuiKey_Tab; imgui_key != ImGuiKey_KeypadEqual; imgui_key = static_cast<ImGuiKey>(std::to_underlying(imgui_key) + 1)) {
		longest = std::max(longest, ImGui::CalcTextSize(ImGui::GetKeyName(imgui_key)).x);
	}
	for (ImGuiMouseButton imguibtn = ImGuiMouseButton_Left; imguibtn < ImGuiMouseButton_COUNT; imguibtn++) {
		const std::string text = "Mouse " + std::to_string(imguibtn);
		longest = std::max(longest, ImGui::CalcTextSize(text.c_str()).x);
	}

	return longest;
}

void GUI::InputManager::Key::draw_bind()
{
	// This is the key that the user last requested to be changed.
	// If this == waiting_target, then we are in charge of selecting a new key.
	static Key* waiting_target = nullptr;

	std::string new_label;

	if (this == waiting_target) {
		new_label = "Waiting for input";

		bool found = false;
		for (ImGuiKey imgui_key = ImGuiKey_Tab; imgui_key != ImGuiKey_KeypadEqual; imgui_key = static_cast<ImGuiKey>(std::to_underlying(imgui_key) + 1)) {
			if (ImGui::IsKeyDown(imgui_key)) {
				waiting_target = nullptr;
				method = Method::KEY;
				key = imgui_key == ImGuiKey_Escape ? ImGuiKey_None : imgui_key;
				active = false;
				found = true;
				break;
			}
		}
		if (!found) {
			for (ImGuiMouseButton imguibtn = ImGuiMouseButton_Left; imguibtn < ImGuiMouseButton_COUNT; imguibtn++) {
				if (ImGui::IsMouseDown(imguibtn)) {
					waiting_target = nullptr;
					method = Method::MOUSE;
					mouse_button = imguibtn;
					active = false;
					break;
				}
			}
		}
	} else {
		if (method == Method::KEY && key == ImGuiKey_None) {
			new_label = "Unset";
		} else if (method == Method::KEY) {
			new_label = ImGui::GetKeyName(key);
		} else if (method == Method::MOUSE) {
			new_label = "Mouse " + std::to_string(mouse_button);
		}
	}

	static const float TEXT_WIDTH = calculate_longest_key_name();
	if (ImGui::Button(new_label.c_str(), { TEXT_WIDTH + ImGui::GetStyle().FramePadding.x * 2.0F, 0.0F })) {
		waiting_target = this;
	}
}

void GUI::InputManager::Key::draw_type()
{
	static_assert(std::to_underlying(Type::HOLD) == 0, "Changing 'Type' requires updating combo items");
	static_assert(std::to_underlying(Type::TOGGLE) == 1, "Changing 'Type' requires updating combo items");

	int type = std::to_underlying(this->type);

	// NOTE: This is called from the BindableSetting, which tries to spell out a sentence, these labels need to fit into the following sentence:
	// "Activate [state] when [key] is [YOU ARE HERE]
	static constexpr std::array<const char*, 2> LOCALIZATIONS{
		"held",
		"toggled"
	};

	static const float TEXT_WIDTH
		= std::ranges::max(LOCALIZATIONS | std::ranges::views::transform([](const char* str) {
			  return ImGui::CalcTextSize(str).x;
		  }));
	const float arrow_size = ImGui::GetFrameHeight();
	const float padding = ImGui::GetStyle().FramePadding.x * 2.0F;
	ImGui::SetNextItemWidth(TEXT_WIDTH + arrow_size + padding);

	ImGui::Combo("##Type", &type, LOCALIZATIONS.data(), LOCALIZATIONS.size());
	this->type = static_cast<Type>(type);
}

void GUI::InputManager::register_key(std::weak_ptr<GUI::InputManager::Key> key)
{
	keys.emplace_back(std::move(key));
}

void GUI::InputManager::update_states()
{

	std::erase_if(keys, [](std::weak_ptr<Key>& key_ptr) {
		if (const std::shared_ptr<Key> key = key_ptr.lock()) {
			key->update_state();
			return false;
		}
		return true;
	});
}
