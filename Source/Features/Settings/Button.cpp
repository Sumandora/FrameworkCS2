#include "Button.hpp"

#include "../Setting.hpp"

#include "imgui.h"

#include <functional>
#include <string>
#include <utility>

Button::Button(SettingsHolder* parent, std::string name, std::function<void()> action)
	: Setting(parent, std::move(name))
	, action(std::move(action))
{
}

void Button::render()
{
	if (ImGui::Button(get_name().c_str()))
		action();
}

void Button::serialize(nlohmann::json& /*output_json*/) const
{
}

void Button::deserialize(const nlohmann::json& /*input_json*/)
{
}
