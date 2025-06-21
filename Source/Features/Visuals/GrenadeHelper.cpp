#include "GrenadeHelper.hpp"

#include "../Feature.hpp"

#include "../../Serialization/GrenadeSerialization.hpp"

#include "../../SDK/Entities/CSPlayerPawn.hpp"
#include "../../SDK/GameClass/GameEvent.hpp"

#include "../../Utils/Projection.hpp"

#include "../../Memory.hpp"

#include "nlohmann/json_fwd.hpp"

#include "glm/ext/vector_float3.hpp"

#include "imgui.h"

#include "octree-cpp/OctreeCpp.h"
#include "octree-cpp/OctreeQuery.h"

#include <cctype>
#include <ranges>
#include <string>
#include <string_view>

using namespace Serialization::Grenades;

GrenadeHelper::GrenadeHelper()
	: Feature("Visuals", "Grenade helper")
	// TODO: Verify that this is actually the boundaries that the s2 engine restricts to.
	, grenades({ glm::vec3{ -32768 }, glm::vec3{ 32768 } })
{
}

void GrenadeHelper::event_handler(GameEvent* event)
{
	if (std::string_view{ event->GetName() } != "game_newmap")
		return;

	const std::string_view new_map = event->get_string("mapname");

	if (current_map != new_map)
		for (const Grenade& grenade : parse_grenades_for_map(new_map)) {
			grenades.Add({ .Vector = grenade.position, .Data = grenade });
		}
}

void GrenadeHelper::draw(ImDrawList* draw_list)
{
	if (!enabled.get())
		return;

	if (!Memory::local_player)
		return;

	const glm::vec3 origin = Memory::local_player->old_origin();

	auto close_grenades = grenades.Query(Octree::Sphere{ .Midpoint = origin, .Radius = render_distance.get() })
		| std::ranges::views::transform([](const Octree::TDataWrapper& data_wrapper) {
			  return data_wrapper.Data;
		  });

	for (const Grenade& grenade : close_grenades) {
		const glm::vec3 pos{ grenade.position[0], grenade.position[1], grenade.position[2] };
		const std::string name = grenade.name.from + " => " + grenade.name.to;
		ImVec2 screen_pos;
		if (!Projection::project(pos, screen_pos))
			continue;
		draw_list->AddText(screen_pos, -1, name.c_str());
	}
}
