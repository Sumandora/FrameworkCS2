#include "FloatSlider.hpp"

#include "../Setting.hpp"

#include "imgui.h"

#include <numeric>
#include <string>
#include <utility>

FloatSlider::FloatSlider(SettingsHolder* parent, std::string name, float min, float max)
	: FloatSlider(parent, std::move(name), min, max, std::midpoint(min, max))
{
}

FloatSlider::FloatSlider(SettingsHolder* parent, std::string name, float min, float max, float value)
	: Setting(parent, std::move(name))
	, min(min)
	, max(max)
	, value(value)
{
}

void FloatSlider::render()
{
	ImGui::SliderFloat(get_name().c_str(), &value, min, max);
}

void FloatSlider::serialize(nlohmann::json& output_json) const
{
	output_json = value;
}

void FloatSlider::deserialize(const nlohmann::json& input_json)
{
	value = input_json;
}
