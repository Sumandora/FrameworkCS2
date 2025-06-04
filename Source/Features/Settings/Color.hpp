#pragma once

#include "../Setting.hpp"

#include "imgui.h"

#include <string>

class Color : public Setting {
	ImColor value;

public:
	Color(SettingsHolder* parent, std::string name, ImColor value = { 1.0F, 1.0F, 1.0F, 1.0F });

	[[nodiscard]] ImColor get() const { return value; }

	void render() override;
	void serialize(nlohmann::json& output_json) const override;
	void deserialize(const nlohmann::json& input_json) override;
};

