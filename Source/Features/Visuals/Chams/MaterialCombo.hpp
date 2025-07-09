#pragma once

#include "../../Setting.hpp"

struct Material;

class MaterialCombo : public Setting {
	std::string material_name;
	mutable Material** material = nullptr;
	mutable bool material_has_changed;

	bool update_material() const; // welp
public:
	MaterialCombo(SettingsHolder* parent, std::string name);
	~MaterialCombo() override;

	[[nodiscard]] Material* get() const;

	void render() override;
	void serialize(nlohmann::json& output_json) const override;
	void deserialize(const nlohmann::json& input_json) override;
};

