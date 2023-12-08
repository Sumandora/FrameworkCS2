//
// Created by snek on 12/8/23.
//

#include "Label.hpp"

#include "../Utils/Spacing.hpp"

#include <imgui.h>
#include <utility>

namespace Menu {
	Label::Label(std::string text)
		: text(std::move(text))
	{
	}

	void Label::draw()
	{
		ImGui::TextUnformatted(text.c_str());
		addSpacing(-4);
	}
} // Menu