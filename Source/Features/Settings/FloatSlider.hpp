#pragma once

#include "../BindableSetting.hpp"
#include "../Setting.hpp"

#include <atomic>
#include <string>

class RawFloatSlider : public Setting {
	float min, max;
	std::atomic<float> value;

public:
	RawFloatSlider(SettingsHolder* parent, std::string name, float min, float max);
	RawFloatSlider(SettingsHolder* parent, std::string name, float min, float max, float value);

	[[nodiscard]] float get() const { return value.load(std::memory_order::relaxed); }

	void render() override;
	void serialize(nlohmann::json& output_json) const override;
	void deserialize(const nlohmann::json& input_json) override;
};

using FloatSlider = BindableSetting<RawFloatSlider>;
