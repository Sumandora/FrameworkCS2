#pragma once

#include "../BindableSetting.hpp"
#include "../Setting.hpp"

#include <atomic>
#include <string>

class RawFloatDrag : public Setting {
	float speed;
	std::atomic<float> value;

public:
	RawFloatDrag(SettingsHolder* parent, std::string name, float speed, float value);

	[[nodiscard]] float get() const { return value.load(std::memory_order::relaxed); }

	void render() override;
	void serialize(nlohmann::json& output_json) const override;
	void deserialize(const nlohmann::json& input_json) override;
};

using FloatDrag = BindableSetting<RawFloatDrag>;
