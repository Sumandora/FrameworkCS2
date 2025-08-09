#include "WorldColors.hpp"

#include "../../Features/Feature.hpp"

#include "../../SDK/Color.hpp"
#include "../../SDK/GameClass/GameEvent.hpp"
#include "../../SDK/GameClass/MeshDrawPrimitive.hpp"
#include "../../SDK/GameClass/SceneLightObject.hpp"
#include "../../SDK/Padding.hpp"

#include "imgui.h"

#include "gch/small_vector.hpp"

#include <cstring>
#include <functional>
#include <optional>
#include <string_view>

WorldColors::WorldColors()
	: Feature("Visuals", "World colors")
{
}

void WorldColors::handle_particle(MeshDrawPrimitive* particle_draw_primitives, int count) const
{
	if (!change_inferno_color.get())
		return;

	std::optional<ImColor> molotov_color;
	std::optional<ImColor> incendiary_color;

	for (int i = 0; i < count; i++) {
		MeshDrawPrimitive& particle = particle_draw_primitives[i];
		ParticleCollection* particle_collection = particle.particle_collection;
		if (!particle_collection)
			continue;

		ParticleSystemDefinition* particle_system_definition = particle.particle_collection->particle_system_definition;
		if (!particle_system_definition)
			continue;
		if (!particle_system_definition->name)
			continue;

		const char* name = *particle_system_definition->name;

		if (std::strstr(name, "molotov")) {
			if (!molotov_color.has_value()) {
				molotov_color = this->molotov_color.get();
			}
			particle.color = molotov_color.value();
			continue;
		}

		if (std::strstr(name, "incendiary")) {
			if (!incendiary_color.has_value()) {
				incendiary_color = this->incendiary_color.get();
			}
			particle.color = incendiary_color.value();
			continue;
		}
	}
}

void WorldColors::handle_light(SceneLightObject* scene_light_object)
{
	if (!change_light_color.get()) {
		if (auto it = original_light_colors.find(scene_light_object);
			it != original_light_colors.end()) {
			scene_light_object->r = it->second.r;
			scene_light_object->g = it->second.g;
			scene_light_object->b = it->second.b;
			original_light_colors.erase(it);
		}
		return;
	}

	const ImColor color = light_color.get();

	original_light_colors.try_emplace(scene_light_object,
		RGB{
			.r = scene_light_object->r,
			.g = scene_light_object->g,
			.b = scene_light_object->b,
		});

	// Since light can't really be transparent, we can reuse alpha as a sort of intensity modifier, at least it doesn't go unused this way.
	scene_light_object->r = color.Value.x * color.Value.w;
	scene_light_object->g = color.Value.y * color.Value.w;
	scene_light_object->b = color.Value.z * color.Value.w;
}

bool WorldColors::handle_sky(MeshDrawPrimitive* sky_draw_primitives, int count,
	const std::function<void(MeshDrawPrimitive*, int)>& draw_mesh) const
{
	if (!change_sky_color.get())
		return false;

	const ImColor color = sky_color.get();

	gch::small_vector<ImColor, 4> previous_colors(count);

	struct SkyBoxObject {
		// string xref: "SkyTint"
		PADDING(0x120);
		float r, g, b, a;
	};

	for (int i = 0; i < count; i++) {
		auto* sky_box_object = reinterpret_cast<SkyBoxObject*>(sky_draw_primitives[i].scene_animatable_object);
		previous_colors[i] = { sky_box_object->r, sky_box_object->g, sky_box_object->b, sky_box_object->a };
		sky_box_object->r = color.Value.x;
		sky_box_object->g = color.Value.y;
		sky_box_object->b = color.Value.z;
		sky_box_object->a = color.Value.w;
	}

	draw_mesh(sky_draw_primitives, count);

	for (int i = 0; i < count; i++) {
		auto* sky_box_object = reinterpret_cast<SkyBoxObject*>(sky_draw_primitives[i].scene_animatable_object);
		const ImColor& original_color = previous_colors[i];
		sky_box_object->r = original_color.Value.x;
		sky_box_object->g = original_color.Value.y;
		sky_box_object->b = original_color.Value.z;
		sky_box_object->a = original_color.Value.w;
	}

	return true;
}

void WorldColors::event_handler(GameEvent* event)
{
	if (original_light_colors.empty())
		return;

	// TODO No clue where to write this down, but I should probably stop call GetName() all the time...
	if (std::string_view{ event->GetName() } != "game_newmap")
		return;

	// New map... well our old lights wont matter on the new one, so dump them.
	original_light_colors.clear();
}
