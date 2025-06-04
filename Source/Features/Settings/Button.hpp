#pragma once

#include "../Setting.hpp"

#include <functional>
#include <string>

class Button : public Setting {
	std::function<void()> action;

public:
	Button(SettingsHolder* parent, std::string name, std::function<void()> action);

	void render() override;
	void serialize(nlohmann::json& output_json) const override;
	void deserialize(const nlohmann::json& input_json) override;
};

