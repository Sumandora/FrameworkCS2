#include "HelpMarker.hpp"

#include "../Setting.hpp"

#include "../../GUI/Elements/HelpMarker.hpp"

#include <string>
#include <utility>

HelpMarker::HelpMarker(SettingsHolder* parent, std::string description)
	: Setting(parent, "Help Marker")
	, description(std::move(description))
{
}

void HelpMarker::render()
{
	ImGuiExt::HelpMarker(description.c_str());
}

void HelpMarker::serialize(nlohmann::json& /*output_json*/) const
{
	// TODO don't save a null value.
}

void HelpMarker::deserialize(const nlohmann::json& /*input_json*/)
{
}
