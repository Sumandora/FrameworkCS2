#pragma once

#include "../Setting.hpp"
#include "../Instrumentation/InstrumentableSetting.hpp"

#include <atomic>
#include <string>

class RawCheckbox : public Setting {
	std::atomic<bool> value;

public:
	RawCheckbox(SettingsHolder* parent, std::string name, bool value);

	[[nodiscard]] bool get() const { return value.load(std::memory_order::relaxed); }

	void render() override;
	void serialize(nlohmann::json& output_json) const override;
	void deserialize(const nlohmann::json& input_json) override;
};

using Checkbox = InstrumentableSetting<RawCheckbox>;
