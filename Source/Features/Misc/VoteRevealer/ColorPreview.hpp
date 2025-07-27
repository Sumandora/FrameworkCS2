#pragma once

#include "../../Setting.hpp"

#include "../../Settings/Color.hpp"

#include <string>

class ColorPreview : public Setting {
	Color& announce_color;
	Color& issued_by_color;
	Color& vote_cast_color;
	Color& yes_color;
	Color& no_color;
	Color& player_highlight_color;

public:
	ColorPreview(SettingsHolder* parent, std::string name,
		Color& announce_color,
		Color& issued_by_color,
		Color& vote_cast_color,
		Color& yes_color,
		Color& no_color,
		Color& player_highlight_color);

	void render() override;
	void serialize(nlohmann::json& /*output_json*/) const override { }
	void deserialize(const nlohmann::json& /*input_json*/) override { }
};
