#pragma once

#include "../Setting.hpp"

#include <string>

class Tabs : public Setting, public SettingsHolder {
public:
	explicit Tabs(SettingsHolder* parent, std::string name);

	void render() override;
	void serialize(nlohmann::json& output_json) const override;
	void deserialize(const nlohmann::json& input_json) override;
};
