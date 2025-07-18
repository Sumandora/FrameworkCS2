#include "MetaSetting.hpp"

#include "../Setting.hpp"

#include <string>
#include <utility>

MetaSetting::MetaSetting(SettingsHolder* parent, std::string name)
	: Setting(parent, std::move(name))
{
}

void MetaSetting::render()
{
	render_all_childs();
}

void MetaSetting::serialize(nlohmann::json& output_json) const
{
	SettingsHolder::serialize(output_json);
}

void MetaSetting::deserialize(const nlohmann::json& input_json)
{
	SettingsHolder::deserialize(input_json);
}
