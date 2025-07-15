#pragma once

#include "../Setting.hpp"

#include <string>

class Section : public Setting, public SettingsHolder {
public:
	Section(SettingsHolder* parent, std::string name);

	void render() override;
	void serialize(nlohmann::json& output_json) const override;
	void deserialize(const nlohmann::json& input_json) override;
};

