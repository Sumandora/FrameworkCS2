#pragma once

#include "../Setting.hpp"
#include "../Instrumentation/InstrumentableSetting.hpp"

#include <string>

class RawTextInput : public Setting {
	std::string value;

public:
	RawTextInput(SettingsHolder* parent, std::string name, std::string default_value);

	[[nodiscard]] const std::string& get() const { return value; }

	void render() override;
	void serialize(nlohmann::json& output_json) const override;
	void deserialize(const nlohmann::json& input_json) override;
};

using TextInput = InstrumentableSetting<RawTextInput>;
