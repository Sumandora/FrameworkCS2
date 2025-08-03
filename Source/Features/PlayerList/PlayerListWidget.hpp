#pragma once

#include "../Setting.hpp"

#include <string>

class PlayerList;

class PlayerListWidget : public Setting {
	PlayerList* player_list;

public:
	PlayerListWidget(SettingsHolder* parent, std::string name, PlayerList* player_list);

	void render() override;
	void serialize(nlohmann::json& /*output_json*/) const override { }
	void deserialize(const nlohmann::json& /*input_json*/) override { }
};
