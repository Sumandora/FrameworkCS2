#include "Label.hpp"

#include "../Utils/Spacing.hpp"

#include <imgui.h>
#include <utility>

namespace Menu {
	Label::Label(std::string name)
		: Widget(std::move(name))
	{
	}

	void Label::draw()
	{
		ImGui::TextUnformatted(name.c_str());
		addSpacing(-4);
	}
} // Menu