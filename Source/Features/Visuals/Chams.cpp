#include "Chams.hpp"

#include "../Feature.hpp"

#include "../../SDK/Entities/CSPlayerPawn.hpp"
#include "../../SDK/EntityHandle.hpp"
#include "../../SDK/GameClass/MeshDrawPrimitive.hpp"
#include "../../SDK/Material/Material.hpp"
#include "../../SDK/Material/MaterialSystem.hpp"

#include "../../Utils/Logging.hpp"

#include "../../Interfaces.hpp"

#include "../../Memory.hpp"

#include "imgui.h"

#include <cstring>
#include <functional>

static Material* my_material = nullptr;

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

Chams::Chams()
	: Feature("Visuals", "Chams")
{
	my_material = Interfaces::material_system->create_material("Lotto otto", szVMatBufferWhiteVisible);

	Logging::info("Created material: {}", my_material);
	Logging::info("Material name: {}", my_material->get_name());
}

bool Chams::draw_object(MeshDrawPrimitive* meshes, int count, const std::function<void(MeshDrawPrimitive*, int)>& draw_mesh)
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
		mesh_draw_primitive.material = my_material;
		mesh_draw_primitive.color = tint_color;
	}

	draw_mesh(meshes, count);

	return true;
}
