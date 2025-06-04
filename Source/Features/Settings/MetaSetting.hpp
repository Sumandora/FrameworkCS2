#pragma once

#include "../Setting.hpp"

#include <string>

class MetaSetting : public Setting, public SettingsHolder {
public:
	MetaSetting(SettingsHolder* parent, std::string name);

	void render() override;
	void serialize(nlohmann::json& output_json) const override;
	void deserialize(const nlohmann::json& input_json) override;
};

