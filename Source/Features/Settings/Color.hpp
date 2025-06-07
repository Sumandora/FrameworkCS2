#pragma once

#include "../Setting.hpp"
#include "../Conditions/SettingWithConditions.hpp"

#include "imgui.h"

#include <string>

class RawColor : public Setting {
	ImColor value;

public:
	RawColor(SettingsHolder* parent, std::string name, ImColor value = { 1.0F, 1.0F, 1.0F, 1.0F });

	[[nodiscard]] ImColor get() const { return value; }

	void render() override;
	void serialize(nlohmann::json& output_json) const override;
	void deserialize(const nlohmann::json& input_json) override;
};

using Color = InstrumentableSetting<RawColor>;
