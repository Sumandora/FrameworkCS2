#pragma once
#include "ButtonArray.hpp"
#include "Widget.hpp"

#include <imgui.h>

#include "../../Config/Setting.hpp"

#include <string>

namespace Menu {
	class ColorPicker final : public Widget {
	public:
		ColorPicker(std::string name, Config::Setting<ImU32>& setting, bool noAlpha = false);

		void draw() override;
		void drawInline();

		[[nodiscard]] static float width();

	private:
		const std::string mainPopupLabel;
		Config::Setting<ImU32>& setting;
		const bool noAlpha;
		ButtonArray copyPasteButtons;
	};
}