#pragma once

#include "Key.hpp"
#include "Setting.hpp"

#include <imgui.h>

namespace Config {
	inline std::vector<Keybind*> keybinds;

	template <typename T>
	void addKeybind(Setting<T>& value, KeybindWithValue<T>* keybind)
	{
		value.keybinds.push_back(keybind);
		keybinds.push_back(keybind);
	}

	void addKeybind(KeybindButton* keybind);

	template <typename T>
	void removeKeybind(Setting<T>& value, KeybindWithValue<T>* keybind)
	{
		auto it = std::find(value.keybinds.begin(), value.keybinds.end(), keybind);
		if (it != value.keybinds.end()) {
			value.keybinds.erase(it);
		}
		it = std::find(keybinds.begin(), keybinds.end(), keybind);
		if (it != keybinds.end()) {
			keybinds.erase(it);
		}
		delete keybind;
	}

	void processInput();

	enum class DPI {
		X1,
		X1_25,
		X1_5,
		X2,
	};

	struct Menu {
		Setting<ImU32> accentColor { IM_COL32(66, 150, 249, 255) };
		Setting<Key> openKey { Key::fromKey(ImGuiKey_Insert) };
		Setting<DPI> dpi { DPI::X1 };
	};

	struct C {
		Menu menu;
	};

	inline C c;

	void save(std::string path);
	void load(std::string path);

	float getDpiScale(DPI dpi = *c.menu.dpi);
}