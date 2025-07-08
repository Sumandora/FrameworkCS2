#pragma once

#include "../Setting.hpp"

#include <string>

class Text : public Setting {
	std::string content;

public:
	Text(SettingsHolder* parent, std::string content);

	void render() override;
	void serialize(nlohmann::json& output_json) const override;
	void deserialize(const nlohmann::json& input_json) override;
};
