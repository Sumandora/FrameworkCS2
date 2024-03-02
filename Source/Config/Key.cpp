#include "Key.hpp"

#include <mutex>

#include "../Utils/Log.hpp"

#include <imgui.h>

namespace Config {
	Key Key::disabled()
	{
		return Key(1);
	}

	Key Key::fromKey(const ImGuiKey key)
	{
		return Key(key);
	}

	Key Key::fromMouseButton(const int mouseButton)
	{
		if (mouseButton > 5) {
			static std::once_flag warningFlag;
			std::call_once(warningFlag, [mouseButton] {
				log(Log::Level::Error, "Mouse button {} is > 5, which is not supported by ImGui", mouseButton);
			});
		}
		// Could actually store the mouse button as is,
		// because the first 512 keys are reserved for legacy compatibility.
		// However, in my opinion, this is more explicit.
		return Key(-mouseButton);
	}

	Key::Key(const int code)
		: code(code)
	{
	}

	bool Key::isDown() const
	{
		if (code == 1) {
			return false;
		}
		if (code > 0) {
			return ImGui::IsKeyDown(static_cast<ImGuiKey>(code));
		}
		const int mouseButton = -code;
		const bool* buttons = ImGui::GetIO().MouseDown;
		return buttons[mouseButton];
	}

	std::string_view Key::toString() const
	{
		if (code == 1) {
			static constexpr std::string_view unbound = "Unbound";
			return unbound;
		}
		if (code > 0) {
			return ImGui::GetKeyName(static_cast<ImGuiKey>(code));
		}
		static constexpr std::string_view mouseButtons[] = {
			"Left Mouse Button",
			"Right Mouse Button"
			"Middle Mouse Button"
			"Mouse4",
			"Mouse5",
		};
		const int mouseButton = -code;
		return mouseButtons[mouseButton];
	}

	void Key::serialize(libconfig::Setting& settingsGroup) const {
		settingsGroup["code"] = code;
	}

	void Key::deserialize(const libconfig::Setting& settingsGroup) {
		settingsGroup.lookupValue("code", code);
	}
}