#include "InputManager.hpp"

#include "GUI.hpp"

#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"

#include <cfloat>
#include <memory>
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

void GUI::InputManager::Key::draw()
{
	// This is the key that the user last requested to be changed.
	// If this == waiting_target, then we are in charge of selecting a new key.
	static Key* waiting_target;

	std::string new_label;

	if (this == waiting_target) {
		new_label = "Waiting for input";

		bool found = false;
		for (ImGuiKey imgui_key = ImGuiKey_Tab; imgui_key != ImGuiKey_KeypadEqual; imgui_key = static_cast<ImGuiKey>(static_cast<int>(imgui_key) + 1)) {
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

	if (ImGui::Button(new_label.c_str(), { -FLT_MIN, 0 })) {
		waiting_target = this;
	}

	static_assert(std::to_underlying(Type::HOLD) == 0, "Changing 'Type' requires updating combo items");
	static_assert(std::to_underlying(Type::TOGGLE) == 1, "Changing 'Type' requires updating combo items");

	int type = std::to_underlying(this->type);
	ImGui::Combo("##Type", &type, "Hold\0Toggle\0");
	this->type = static_cast<Type>(type);

#if 0
	// TODO Enable once there is an active keybinds list.
	ImGui::InputTextWithHint("##Name", "Name", &name);
	ImGui::SetItemTooltip("Name for active keybinds list. Empty name implies hidden.");
#endif
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

GUI::InputManager& GUI::get_input_manager()
{
	static InputManager input_manager{};
	return input_manager;
}
