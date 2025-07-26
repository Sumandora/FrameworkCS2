#include "Checkbox.hpp"

#include "../Setting.hpp"

#include "imgui.h"

#include <atomic>
#include <string>
#include <utility>

RawCheckbox::RawCheckbox(SettingsHolder* parent, std::string name, bool value)
	: Setting(parent, std::move(name))
	, value(value)
{
}

void RawCheckbox::render()
{
	bool value = this->value.load(std::memory_order::acquire);
	if (ImGui::Checkbox(get_name().c_str(), &value))
		this->value.store(value);
}

void RawCheckbox::serialize(nlohmann::json& output_json) const
{
	output_json = value.load();
}

void RawCheckbox::deserialize(const nlohmann::json& input_json)
{
	value.store(input_json.get<bool>());
}
