#include "MaterialCombo.hpp"

#include "../../../SDK/GameClass/MeshDrawPrimitive.hpp"
#include "../../../SDK/GameClass/ResourceHandleUtils.hpp"
#include "../../../SDK/Material/Material.hpp"
#include "../../../SDK/Material/MaterialSystem.hpp"

#include "../../../Utils/Logging.hpp"

#include "../../../Interfaces.hpp"

#include "../../../Serialization/Materials.hpp"

#include "../../../Notifications/Notifications.hpp"

#include "../../Setting.hpp"

#include "imgui.h"

#include <algorithm>
#include <cstring>
#include <format>
#include <functional>
#include <string>
#include <unistd.h>
#include <utility>

static void delete_material(Material** material)
{
	ResourceHandleUtils* resource_handle_utils = ResourceHandleUtils::get();

	if (!resource_handle_utils) {
		Logging::warn("Leaking memory because material '{}' can't be deleted without ResourceHandleUtils", (*material)->get_name());
		return;
	}

	resource_handle_utils->delete_resource(reinterpret_cast<void*>(material));
}

MaterialCombo::MaterialCombo(SettingsHolder* parent, std::string name)
	: Setting(parent, std::move(name))
	, material_has_changed(false)
{
	const auto& materials = Serialization::Materials::get_materials();
	if (!materials.empty()) {
		material_name = materials.at(0).name;
		material_has_changed = true;
	}
}

MaterialCombo::~MaterialCombo()
{
	if (!material)
		return;

	delete_material(material);
	material = nullptr;
}

bool MaterialCombo::update_material() const
{
	material_has_changed = false; // Even if this fails, we don't want to be called again

	if (material_name.empty()) {
		if (material) {
			delete_material(material);
		}
		material = nullptr;
		return true;
	}

	auto materials = Serialization::Materials::get_materials();
	auto it = std::ranges::find(
		materials,
		material_name,
		[](const Serialization::Materials::Material& material) { return material.name; });

	if (it == materials.end()) {
		Notifications::create(
			"Material Creation",
			std::format("Attempted to load material '{}', but material is no longer available...", material_name),
			Notifications::Severity::ERROR);
		return false;
	}
	const std::string kv3 = it->acquire_kv3();
	if (kv3.empty()) {
		Notifications::create(
			"Material Creation",
			std::format("Attempted to load material '{}', but kv3 is no longer available...", material_name),
			Notifications::Severity::ERROR);
		return false;
	}

	if (material) {
		delete_material(material);
	}

	material = Interfaces::material_system->create_material(material_name.c_str(), kv3.c_str());

	if (material)
		Logging::info("Created material '{}' at {} -> {}", material_name, material, *material);
	else {
		Notifications::create(
			"Material Creation",
			std::format("Failed to create material '{}'", material_name),
			Notifications::Severity::ERROR);
	}

	return true;
}

Material* MaterialCombo::get() const
{
	if (material_has_changed)
		update_material();

	if (!material)
		return nullptr;

	return *material;
}

void MaterialCombo::render()
{
	static constexpr const char* UNCHANGED = "Unchanged";
	if (ImGui::BeginCombo(get_name().c_str(), material_name.empty() ? UNCHANGED : material_name.c_str(), ImGuiComboFlags_None)) {
		if (ImGui::Selectable(UNCHANGED, material_name.empty())) {
			material_name = {};
			material_has_changed = true;
		}
		for (const Serialization::Materials::Material& material : Serialization::Materials::get_materials()) {
			const bool selected = material.name == material_name;
			if (ImGui::Selectable(material.name.c_str(), selected)) {
				material_name = material.name;
				material_has_changed = true;
			}

			if (selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
}
void MaterialCombo::serialize(nlohmann::json& output_json) const
{
	output_json = material_name;
}
void MaterialCombo::deserialize(const nlohmann::json& input_json)
{
	material_name = input_json;
	material_has_changed = true;
}
