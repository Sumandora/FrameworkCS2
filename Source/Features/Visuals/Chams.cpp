#include "Chams.hpp"

#include "../Feature.hpp"

#include "../../SDK/Entities/CSPlayerPawn.hpp"
#include "../../SDK/EntityHandle.hpp"
#include "../../SDK/GameClass/EnginePVSManager.hpp"
#include "../../SDK/GameClass/MeshDrawPrimitive.hpp"
#include "../../SDK/GameClass/ResourceHandleUtils.hpp"
#include "../../SDK/GameClass/ResourceSystem.hpp"
#include "../../SDK/Material/Material.hpp"
#include "../../SDK/Material/MaterialSystem.hpp"

#include "../../Utils/Logging.hpp"

#include "../../Interfaces.hpp"

#include "../../Notifications/Notifications.hpp"

#include "../../Serialization/Materials.hpp"

#include "../../Memory.hpp"

#include "../Setting.hpp"

#include "BCRL/SearchConstraints.hpp"
#include "BCRL/Session.hpp"

#include "SignatureScanner/PatternSignature.hpp"
#include "SignatureScanner/XRefSignature.hpp"

#include "imgui.h"

#include <algorithm>
#include <chrono>
#include <cstring>
#include <format>
#include <functional>
#include <string>
#include <unistd.h>
#include <utility>

static EnginePVSManager* engine_pvs_manager = nullptr;

static ResourceHandleUtils* resource_handle_utils = nullptr;

static void delete_material(Material** material)
{
	if (!resource_handle_utils) {
		Logging::warn("Leaking memory because material '{}' can't be deleted without ResourceHandleUtils", (*material)->get_name());
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
	if (ImGui::BeginCombo(get_name().c_str(), material_name.c_str(), ImGuiComboFlags_None)) {
		if (ImGui::Selectable("Unchanged", material_name.empty())) {
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

Chams::Chams()
	: Feature("Visuals", "Chams")
{
	engine_pvs_manager
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_literal_string<"vis_sunlight_enable">(),
			BCRL::everything(Memory::mem_mgr).thats_readable().with_name("libengine2.so"))
			  .find_xrefs(SignatureScanner::XRefTypes::relative(),
				  BCRL::everything(Memory::mem_mgr).thats_readable().with_name("libengine2.so"))
			  .next_signature_occurrence(SignatureScanner::PatternSignature::for_array_of_bytes<"48 89 05 ?? ?? ?? ?? 48">())
			  .add(3)
			  .relative_to_absolute()
			  .finalize<EnginePVSManager*>()
			  .value_or(nullptr);

	if (!engine_pvs_manager) {
		Logging::error("Couldn't find EnginePVSManager. Chams will not be able to disable it.");

		disable_pvs.add_visible_condition([] { return false; });
		pvs_help.add_visible_condition([] { return false; });
	}

	resource_handle_utils = reinterpret_cast<ResourceHandleUtils*>(Interfaces::resource_system->query_interface("ResourceHandleUtils001"));

	if (!resource_handle_utils) {
		Logging::warn("Wasn't able to acquire ResourceHandleUtils, switching Chams textures will memory leak.");
	}
}

void Chams::update_pvs() const
{
	if (!engine_pvs_manager)
		return;

	// Don't update it every frame, that might result in a bigger performance drop than it should be.
	using std::chrono::system_clock;
	static system_clock::time_point last_update = system_clock::now();
	const system_clock::time_point right_now = system_clock::now();
	if (right_now - last_update > std::chrono::seconds(5)) {
		engine_pvs_manager->enable(!disable_pvs.get());
		last_update = right_now;
	}
}

bool Chams::draw_object(MeshDrawPrimitive* meshes, int count, const std::function<void(MeshDrawPrimitive*, int)>& draw_mesh) const
{
	if (!enabled.get())
		return false;

	for (const Layer* layer : layers.get()) {
		const ImColor tint_color = layer->tint.get();

		for (int i = 0; i < count; i++) {
			// TODO ideally it would cache if a MeshDrawPrimitive is eligible for enhancement
			MeshDrawPrimitive& mesh_draw_primitive = meshes[i];
			if (!mesh_draw_primitive.scene_animatable_object)
				continue;
			const EntityHandle<BaseEntity> owner = mesh_draw_primitive.scene_animatable_object->owner;
			if (!owner.has_entity())
				continue;
			BaseEntity* ent = owner.get();
			// TODO When changing the material of the player while in first person, the legs and shadow disappear
			if (!ent || Memory::local_player == ent)
				continue;
			if (!ent->entity_cast<CSPlayerPawn*>())
				continue;
			Material* material = layer->material.get();
			if (material)
				mesh_draw_primitive.material = material;
			mesh_draw_primitive.color = tint_color;
		}

		draw_mesh(meshes, count);
	}

	return true;
}
