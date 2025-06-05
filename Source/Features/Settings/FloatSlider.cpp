#include "FloatSlider.hpp"

#include "../Setting.hpp"

#include "imgui.h"

#include <numeric>
#include <string>
#include <utility>

RawFloatSlider::RawFloatSlider(SettingsHolder* parent, std::string name, float min, float max)
	: RawFloatSlider(parent, std::move(name), min, max, std::midpoint(min, max))
{
}

RawFloatSlider::RawFloatSlider(SettingsHolder* parent, std::string name, float min, float max, float value)
	: Setting(parent, std::move(name))
	, min(min)
	, max(max)
	, value(value)
{
}

void RawFloatSlider::render()
{
	ImGui::SliderFloat(get_name().c_str(), &value, min, max);
}

void RawFloatSlider::serialize(nlohmann::json& output_json) const
{
	output_json = value;
}

void RawFloatSlider::deserialize(const nlohmann::json& input_json)
{
	value = input_json;
}
