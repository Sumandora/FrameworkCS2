#include "../Setting.hpp"

#include "imgui.h"

#include "nlohmann/json.hpp"
#include "nlohmann/json_fwd.hpp"

#include <string>
#include <utility>

FloatSlider::FloatSlider(SettingsHolder* parent, std::string name, float min, float max)
	: Setting(parent, std::move(name))
	, min(min)
	, max(max)
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
