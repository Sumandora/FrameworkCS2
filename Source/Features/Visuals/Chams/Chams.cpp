#include "Chams.hpp"

#include "../../Feature.hpp"

#include "../../../SDK/Entities/CSPlayerPawn.hpp"
#include "../../../SDK/EntityHandle.hpp"
#include "../../../SDK/GameClass/EnginePVSManager.hpp"
#include "../../../SDK/GameClass/MeshDrawPrimitive.hpp"
#include "../../../SDK/Material/Material.hpp"
#include "../../../SDK/Material/MaterialSystem.hpp"

#include "../../../Utils/Logging.hpp"
#include "../../../Utils/Interval.hpp"

#include "../../../Memory.hpp"

#include "BCRL/SearchConstraints.hpp"
#include "BCRL/Session.hpp"

#include "SignatureScanner/PatternSignature.hpp"
#include "SignatureScanner/XRefSignature.hpp"

#include "imgui.h"

#include "gch/small_vector.hpp"

#include <chrono>
#include <cstring>
#include <format>
#include <functional>
#include <shared_mutex>
#include <string>
#include <unistd.h>

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
}

void Chams::update_pvs() const
{
	if (!engine_pvs_manager)
		return;

	// Don't update it every frame, that might result in a bigger performance drop than it should be.
	static Interval<std::chrono::seconds, 5> pvs_update_timer;
	if (pvs_update_timer.passed()) {
		engine_pvs_manager->enable(!disable_pvs.get());
	}
}

bool Chams::draw_object(MeshDrawPrimitive* meshes, int count, const std::function<void(MeshDrawPrimitive*, int)>& draw_mesh) const
{
	if (!enabled.get())
		return false;

	const std::shared_lock layers_lock = layers.read_lock();

	for (const Layer* layer : layers.get()) {
		const ImColor tint_color = layer->tint.get();

		gch::small_vector<Material*, 4> previous_materials(count);

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
			previous_materials[i] = mesh_draw_primitive.material;
			Material* material = layer->material.get();
			if (material) {
				mesh_draw_primitive.material = material;
			}
			mesh_draw_primitive.color = tint_color;
		}

		draw_mesh(meshes, count);

		// Restore original material for next layer
		for (int i = 0; i < count; i++) {
			meshes[i].material = previous_materials[i];
		}
	}

	return true;
}
