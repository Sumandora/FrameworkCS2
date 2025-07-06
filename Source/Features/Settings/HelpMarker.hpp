#pragma once

#include "../Setting.hpp"

#include <string>

class HelpMarker : public Setting {
	std::string description;

public:
	HelpMarker(SettingsHolder* parent, std::string description);

	void render() override;
	void serialize(nlohmann::json& output_json) const override;
	void deserialize(const nlohmann::json& input_json) override;
};
