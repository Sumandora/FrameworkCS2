#include "Glow.hpp"

#include "../Feature.hpp"

#include "../../SDK/Entities/BaseEntity.hpp"
#include "../../SDK/Entities/CSPlayerPawn.hpp"

#include "../../Memory.hpp"

#include "imgui.h"

Glow::Glow()
	: Feature("Visuals", "Glow")
{
}

bool Glow::wants_modify(BaseEntity* entity)
{
	if (!entity)
		// The game asks ClientModeCSNormal if the entity is null.
		// Not quite sure what effect this has, but the game also calls int3 right after, a valve intern probably forgot to return.
		// This state is not normal and if the game ever encounters a glow property with no entity, but with glowing == true, it will crash.
		return false;

	if (!enabled.get())
		return false;

	if (entity == Memory::local_player)
		return false;

	return entity->getSchemaType() == CSPlayerPawn::classInfo();
}

ImColor Glow::get_color_for_entity(
	// TODO node circuit needs to take contextual entities, I can't keep pushing this in front of myself...
	[[maybe_unused]] BaseEntity* entity)
{
	return color.get();
}
