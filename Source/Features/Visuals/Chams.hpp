#pragma once

#include "../Feature.hpp"

#include "../Settings/Checkbox.hpp"
#include "../Settings/Text.hpp"
#include "../Settings/Color.hpp"
#include "../Settings/DynamicTabs.hpp"
#include "../Settings/HelpMarker.hpp"
#include "../Settings/MetaSetting.hpp"

#include "../../Utils/UninitializedObject.hpp"

#include "../Setting.hpp"
#include "imgui.h"

#include <functional>
#include <string>

struct MeshDrawPrimitive;
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

// TODO instrumentation?

class Chams : public Feature {
	Checkbox enabled{ this, "Enabled", false };
	struct Layer : public MetaSetting {
		MaterialCombo material{ this, "Material" };
		Color tint{ this, "Tint color", ImColor{ 1.0F, 0.0F, 0.0F, 1.0F } };

		using MetaSetting::MetaSetting;
	};
	Text reorder_hint{ this, "Reordering the following tabs is supported. They are rendered from left to right."};
	DynamicTabs<Layer> layers{ this, "Layers", "Layer" };
	Checkbox disable_pvs{ this, "Disable PVS", true }; // Most cheats disable this by force, but perhaps people with bad PCs would benefit from it.
	HelpMarker pvs_help{ this, "The PVS is responsible for culling out far away entities, disabling it may result in a performance drop." };

public:
	Chams();

	void update_pvs() const;

	// @returns true if original should be canceled
	bool draw_object(MeshDrawPrimitive* meshes, int count, const std::function<void(MeshDrawPrimitive*, int)>& draw_mesh) const;
};

inline UninitializedObject<Chams> chams;
