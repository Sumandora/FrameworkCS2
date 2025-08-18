#pragma once

#include "../../../SDK/Entities/Components/BodyComponent.hpp"
#include "../../../SDK/Entities/Components/BodyComponentSkeletonInstance.hpp"
#include "../../../SDK/Entities/CSPlayerPawn.hpp"
#include "../../../SDK/GameClass/ModelState.hpp"
#include "../../../SDK/GameClass/SkeletonInstance.hpp"

#include "../../../Utils/Projection.hpp"

#include "GenericESP/Element/Line.hpp"
#include "glm/ext/vector_float3.hpp"
#include "imgui.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <initializer_list>
#include <ranges>
#include <utility>

// TODO Are hardcoded IDs safe here?
static constexpr std::initializer_list<std::pair<int, int>> SKELETON_LINES{
	{ 6, 5 },

	// Arms
	{ 5, 13 },
	{ 13, 14 },
	{ 14, 15 },

	{ 5, 8 },
	{ 8, 9 },
	{ 9, 10 },

	// Torso
	{ 5, 4 },
	{ 4, 3 },
	{ 3, 2 },
	{ 2, 1 },
	{ 1, 0 },

	// Legs
	{ 0, 25 },
	{ 25, 26 },
	{ 26, 27 },

	{ 0, 22 },
	{ 22, 23 },
	{ 23, 24 }
};
// TODO This would be prettier with std::ranges::views::concat, but its c++26
static constexpr std::size_t HIGHEST_SKELETON_INDEX
	= std::max(
		std::ranges::max(SKELETON_LINES | std::ranges::views::keys),
		std::ranges::max(SKELETON_LINES | std::ranges::views::values));

inline void draw_skeleton(CSPlayerPawn* player_pawn, ImDrawList* draw_list, const GenericESP::Line& skeleton_line)
{
	BodyComponent* body_component = player_pawn->body_component();

	const SkeletonInstance& skeleton = static_cast<BodyComponentSkeletonInstance*>(body_component)->skeleton_instance();

	const ModelState& model_state = skeleton.model_state();

	if (model_state.bone_count == 0)
		return;

	// The following code mainly tries to avoid projecting points multiple times.

	// Mind that there are gaps in the skeleton lines indexes, so these can't just be computed upfront
	// So this kind of lazy system is used instead
	struct ScreenSpacePoint {
		ImVec2 coords;
		bool visible : 1 = false;
		bool looked_up : 1 = false;

		void get(glm::vec3 world_space) &
		{
			visible = Projection::project(world_space, coords);
		}
	};
	// Yes, there are holes and yes this is allocating more stack memory than necessary, but it's faster
	std::array<ScreenSpacePoint, HIGHEST_SKELETON_INDEX + 1 /* 0 is also a valid index */> screen_space_vecs{};

	for (const auto& [from, to] : SKELETON_LINES) {
		ScreenSpacePoint& from_screen_space = screen_space_vecs[from];
		if (!from_screen_space.looked_up)
			from_screen_space.get(model_state.bones[from].pos);
		if (!from_screen_space.visible)
			continue;

		ScreenSpacePoint& to_screen_space = screen_space_vecs[to];
		if (!to_screen_space.looked_up)
			to_screen_space.get(model_state.bones[to].pos);
		if (!to_screen_space.visible)
			continue;

		// TODO The shadows should be drawn first and then have a separate pass adding the actual lines, so that the shadows are actually behind the lines
		// -> Fix in GenericESP
		skeleton_line.draw(draw_list, player_pawn, from_screen_space.coords, to_screen_space.coords);
	}
}
