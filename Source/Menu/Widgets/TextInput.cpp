#include "TextInput.hpp"

#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"

#include <limits>

namespace Menu {
	TextInput::TextInput(std::string name, Config::Setting<std::string>& setting)
		: Widget(std::move(name))
		, setting(setting)
	{
	}

	void TextInput::draw()
	{
		ImGui::TextUnformatted(name.c_str());
		ImGui::SameLine();
		ImGui::PushItemWidth(-std::numeric_limits<float>::min());
		ImGui::InputText(getLabel().c_str(), setting.getPointer());
		ImGui::PopItemWidth();
	}
} // Menu