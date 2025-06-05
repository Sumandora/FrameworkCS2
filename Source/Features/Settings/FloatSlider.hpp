#pragma once

#include "../Setting.hpp"

#include "../Conditions/SettingWithConditions.hpp"

#include <string>

class RawFloatSlider : public Setting {
	float min, max, value;

public:
	RawFloatSlider(SettingsHolder* parent, std::string name, float min, float max);
	RawFloatSlider(SettingsHolder* parent, std::string name, float min, float max, float value);

	[[nodiscard]] float get() const { return value; }

	void render() override;
	void serialize(nlohmann::json& output_json) const override;
	void deserialize(const nlohmann::json& input_json) override;
};

using FloatSlider = InstrumentableSetting<RawFloatSlider>;
