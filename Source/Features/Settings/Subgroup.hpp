#pragma once

#include "../Setting.hpp"

#include <string>

class Subgroup : public Setting, public SettingsHolder {
	bool anonymous;

public:
	Subgroup(SettingsHolder* parent, std::string name, bool anonymous = false);

	void render() override;
	void serialize(nlohmann::json& output_json) const override;
	void deserialize(const nlohmann::json& input_json) override;
};
