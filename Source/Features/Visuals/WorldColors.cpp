#include "WorldColors.hpp"

#include "../../Features/Feature.hpp"

#include "../../SDK/GameClass/MeshDrawPrimitive.hpp"

#include "imgui.h"

#include <cstring>
#include <optional>

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
