#pragma once

#include "Feature.hpp"
#include "imgui.h"

#include "../Menu/Components/Group.hpp"

namespace Features {
	class Menu : public Feature {
	public:
		enum class DPI {
			X1,
			X1_25,
			X1_5,
			X2,
		};

	private:
		Config::Setting<ImU32> accentColor{ IM_COL32(66, 150, 249, 255) };
		Config::Setting<Config::Key> openKey{ Config::Key::fromKey(ImGuiKey_Insert) };
		Config::Setting<DPI> dpi{ DPI::X1 };

	public:
		Menu();

		ImColor getAccentColor();
		Config::Key getOpenKey();
		float getDpiScale();

		void imguiRender(::Menu::Group& group);

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(Menu, accentColor, openKey, dpi)
	};
}