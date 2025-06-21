#include "GrenadeHelper.hpp"

#include "../Feature.hpp"

#include "../../Serialization/GrenadeSerialization.hpp"

#include "../../SDK/GameClass/GameEvent.hpp"

#include "../../Utils/Projection.hpp"

#include "nlohmann/json_fwd.hpp"

#include "glm/ext/vector_float3.hpp"

#include "imgui.h"

#include <cctype>
#include <string>
#include <string_view>

using namespace Serialization::Grenades;

GrenadeHelper::GrenadeHelper()
	: Feature("Visuals", "Grenade helper")
{
}

void GrenadeHelper::event_handler(GameEvent* event)
{
	if (std::string_view{ event->GetName() } != "game_newmap")
		return;

	const std::string_view new_map = event->get_string("mapname");

	if (current_map != new_map)
		grenades = parse_grenades_for_map(new_map);
}

void GrenadeHelper::draw(ImDrawList* draw_list)
{
	if (!enabled.get())
		return;

	for (const Grenade& grenade : grenades) {
		const glm::vec3 pos{ grenade.position[0], grenade.position[1], grenade.position[2] };
		const std::string name = grenade.name.from + " => " + grenade.name.to;
		ImVec2 screen_pos;
		if (!Projection::project(pos, screen_pos))
			continue;
		draw_list->AddText(screen_pos, -1, name.c_str());
	}
}
