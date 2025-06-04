#pragma once

#include "../Setting.hpp"

#include <string>

class TextInput : public Setting {
	std::string value;

public:
	TextInput(SettingsHolder* parent, std::string name, std::string default_value);

	[[nodiscard]] const std::string& get() const { return value; }

	void render() override;
	void serialize(nlohmann::json& output_json) const override;
	void deserialize(const nlohmann::json& input_json) override;
};
