#include "FloatSlider.hpp"

#include "../Setting.hpp"

#include "imgui.h"

#include <atomic>
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
	float value = this->value.load(std::memory_order::relaxed);
	if (ImGui::SliderFloat(get_name().c_str(), &value, min, max))
		this->value.store(value);
}

void RawFloatSlider::serialize(nlohmann::json& output_json) const
{
	output_json = value.load();
}

void RawFloatSlider::deserialize(const nlohmann::json& input_json)
{
	value.store(input_json.get<float>());
}
