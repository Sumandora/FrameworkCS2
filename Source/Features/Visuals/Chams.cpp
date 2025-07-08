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

#include "../../Memory.hpp"

#include "BCRL/SearchConstraints.hpp"
#include "BCRL/Session.hpp"

#include "SignatureScanner/PatternSignature.hpp"
#include "SignatureScanner/XRefSignature.hpp"

#include "imgui.h"

#include <chrono>
#include <cstring>
#include <functional>

static Material** my_material = nullptr;

// TEMP: taken from aspyhxia
static constexpr char szVMatBufferWhiteVisible[] =
	R"(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d} format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
{
	shader = "csgo_complex.vfx"

	F_SELF_ILLUM = 1
	F_PAINT_VERTEX_COLORS = 1
	F_TRANSLUCENT = 1
    F_DISABLE_Z_BUFFERING = 1

	g_vColorTint = [ 1.000000, 1.000000, 1.000000, 1.000000 ]
	g_flSelfIllumScale = [ 3.000000, 3.000000, 3.000000, 3.000000 ]
	g_flSelfIllumBrightness = [ 3.000000, 3.000000, 3.000000, 3.000000 ]
    g_vSelfIllumTint = [ 10.000000, 10.000000, 10.000000, 10.000000 ]

	g_tColor = resource:"materials/default/default_mask_tga_fde710a5.vtex"
	g_tNormal = resource:"materials/default/default_mask_tga_fde710a5.vtex"
	g_tSelfIllumMask = resource:"materials/default/default_mask_tga_fde710a5.vtex"
	TextureAmbientOcclusion = resource:"materials/debug/particleerror.vtex"
	g_tAmbientOcclusion = resource:"materials/debug/particleerror.vtex"
})";

static EnginePVSManager* engine_pvs_manager = nullptr;

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

	my_material = Interfaces::material_system->create_material("Lotto otto", szVMatBufferWhiteVisible);

	Logging::info("Created material: {}", my_material);
	Logging::info("Material name: {}", (*my_material)->get_name());
}

Chams::~Chams()
{
	auto* resource_handle_utils
		= reinterpret_cast<ResourceHandleUtils*>(Interfaces::resource_system->query_interface("ResourceHandleUtils001"));
	// NOLINTNEXTLINE(bugprone-multi-level-implicit-pointer-conversion)
	resource_handle_utils->delete_resource(my_material);
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

	const ImColor tint_color = tint.get();

	for (int i = 0; i < count; i++) {
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
		mesh_draw_primitive.material = *my_material;
		mesh_draw_primitive.color = tint_color;
	}

	draw_mesh(meshes, count);

	return true;
}
