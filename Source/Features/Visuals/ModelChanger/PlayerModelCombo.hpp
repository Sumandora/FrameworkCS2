#pragma once

#include "../../Setting.hpp"

#include "../../../GUI/TextureManager.hpp"

#include <string>
#include <vector>

class PlayerModelCombo : public Setting {
public:
	struct DefaultModel {
		std::string agent_name;
		std::string model_path;
		GUI::TextureManager::RawImage image;
		bool is_localized; // Some models don't have a localization, in this case the agent_name will be the localization key
	};

private:
	std::string player_model;

	static std::vector<DefaultModel> default_models;
	static std::vector<std::string> custom_models;

	void draw_fancy_model_selection();

public:
	PlayerModelCombo(SettingsHolder* parent, std::string name);

	[[nodiscard]] const std::string& get() const;

	void render() override;
	void serialize(nlohmann::json& output_json) const override;
	void deserialize(const nlohmann::json& input_json) override;
};
