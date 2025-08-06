#include "WorldColors.hpp"

#include "../../Features/Feature.hpp"

#include "../../SDK/GameClass/GameEvent.hpp"
#include "../../SDK/GameClass/MeshDrawPrimitive.hpp"
#include "../../SDK/GameClass/SceneLightObject.hpp"

#include "imgui.h"

#include <cstring>
#include <optional>
#include <string_view>

WorldColors::WorldColors()
	: Feature("Visuals", "World colors")
{
}

void WorldColors::handle_particle(MeshDrawPrimitive* particle_draw_primitives, int count)
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
