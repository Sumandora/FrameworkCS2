#pragma once

#include "../Feature.hpp"

#include "../Settings/Checkbox.hpp"
#include "../Settings/TextInput.hpp"

#include "../../Utils/UninitializedObject.hpp"

#include "../Setting.hpp"

#include "../../Utils/VTexDecoder.hpp"
#include "imgui.h"

#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

class PlayerModelCombo : public Setting {
public:
	struct DefaultModel {
		std::string agent_name;
		std::string model_path;
		VTexDecoder::RawImage image;
		std::optional<ImTextureID> texture_id;
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

class ModelChanger : public Feature {
	Checkbox enabled{ this, "Enabled", false };
	PlayerModelCombo model{ this, "Model" };

public:
	ModelChanger();

	void update_model();
};

inline UninitializedObject<ModelChanger> model_changer;
