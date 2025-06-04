#pragma once

#include "../Setting.hpp"

#include <string>

class Checkbox : public Setting {
	bool value;

public:
	Checkbox(SettingsHolder* parent, std::string name, bool value);

	[[nodiscard]] bool get() const { return value; }

	void render() override;
	void serialize(nlohmann::json& output_json) const override;
	void deserialize(const nlohmann::json& input_json) override;
};
