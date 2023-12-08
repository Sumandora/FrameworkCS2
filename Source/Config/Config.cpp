#include "Config.hpp"

#include "Keybinds.hpp"

namespace Config {
	void addKeybind(KeybindButton* keybind) { keybinds.push_back(keybind); }

	void processInput()
	{
		for (const auto keybind : keybinds) {
			keybind->processInput();
		}
	}

	float getDpiScale(const DPI dpi)
	{
		switch (dpi) {
			using enum DPI;
		case X1:
			return 1.0f;
		case X1_25:
			return 1.25f;
		case X1_5:
			return 1.5f;
		case X2:
			return 2.0f;
		default: // bleeh
			return 1.0f;
		}
	}
}