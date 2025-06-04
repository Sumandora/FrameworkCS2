#pragma once

#include "../Setting.hpp"

#include <string>

class FloatSlider : public Setting {
	float min, max, value;

public:
	FloatSlider(SettingsHolder* parent, std::string name, float min, float max);
	FloatSlider(SettingsHolder* parent, std::string name, float min, float max, float value);

	[[nodiscard]] float get() const { return value; }

	void render() override;
	void serialize(nlohmann::json& output_json) const override;
	void deserialize(const nlohmann::json& input_json) override;
};

