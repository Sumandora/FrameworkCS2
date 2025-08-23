#include "Inferno.hpp"

#include "../../Setting.hpp"
#include "../../Settings/MetaSetting.hpp"

#include "../../../SDK/Entities/Inferno.hpp"

#include "../../../Utils/Projection.hpp"

#include "GenericESP/UnionedRect.hpp"

#include "glm/ext/vector_float3.hpp"
#include "glm/geometric.hpp"

#include "imgui.h"
#include "imgui_internal.h"

#include "hull/algorithms.hpp"

#include <algorithm>
#include <cstddef>
#include <string>
#include <utility>
#include <vector>

ESPInferno::ESPInferno(SettingsHolder* parent, std::string name)
	: MetaSetting(parent, std::move(name))
{
}

bool ESPInferno::is_enabled() const
{
	return box_enabled.get() || name_enabled.get();
}

void ESPInferno::draw_inferno(ImDrawList* draw_list, Inferno* inferno, const ImRect& screenspace_rect) const
{
	GenericESP::UnionedRect unioned_rect{ screenspace_rect };

	if (box_enabled.get())
		box.draw(draw_list, inferno, unioned_rect);

	if (name_enabled.get()) {
		name.draw(draw_list, inferno, get_name(), unioned_rect);
	}

	// Drawing the convex hull:
	if (!convex_hull_enabled.get() || inferno->fire_count() <= 0)
		return;

	// TODO Cache this:

	std::vector<glm::vec3> fire_positions;
	fire_positions.reserve(inferno->fire_count());
	for (std::size_t i = 0; std::cmp_less(i, inferno->fire_count()); i++) {
		if (inferno->fire_is_burning()[i]) {
			fire_positions.emplace_back(inferno->fire_positions()[i]);
		}
	}

	std::vector<glm::vec3> convex_hull;
	convex_hull.reserve(inferno->fire_count());

	hull::convex::compute(fire_positions, convex_hull);

	// Now we need to expand by the size of each fire point
	static constexpr float INFERNO_FIRE_HALF_WIDTH = 30.0F; // @see game/server/cstrike15/Effects/inferno.cpp
	static constexpr float INFERNO_FIRE_WIDTH = INFERNO_FIRE_HALF_WIDTH * 2.0F;

	// Unsure about how safe this is, but it seems to work well enough:
	const glm::vec3 average
		= std::ranges::fold_left(convex_hull, glm::vec3{}, [](const glm::vec3& a, const glm::vec3& b) { return a + b; })
		/ static_cast<float>(convex_hull.size());

	for (glm::vec3& p : convex_hull) {
		p += glm::normalize(p - average) * INFERNO_FIRE_WIDTH;
	}

	// Draw it:
	std::vector<ImVec2> screenspace_points(convex_hull.size());

	for (std::size_t i = 0; i < screenspace_points.size(); i++) {
		if (!Projection::project(convex_hull.at(i), screenspace_points[i]))
			return; // Drawing the convex hull is not possible.
	}

	this->convex_hull.draw(draw_list, inferno, screenspace_points.data(), screenspace_points.size());
}
