#include "FloatDrag.hpp"

#include "../Setting.hpp"

#include "imgui.h"

#include <atomic>
#include <string>
#include <utility>

RawFloatDrag::RawFloatDrag(SettingsHolder* parent, std::string name, float speed, float value)
	: Setting(parent, std::move(name))
	, speed(speed)
	, value(value)
{
}

void RawFloatDrag::render()
{
	float value = this->value.load(std::memory_order::relaxed);
	if (ImGui::DragFloat(get_name().c_str(), &value, speed))
		this->value.store(value);
}

void RawFloatDrag::serialize(nlohmann::json& output_json) const
{
	output_json = value.load();
}

void RawFloatDrag::deserialize(const nlohmann::json& input_json)
{
	value.store(input_json.get<float>());
}
