#pragma once

#include "../Feature.hpp"

#include "../Settings/Checkbox.hpp"
#include "../Settings/Color.hpp"
#include "../Settings/FloatSlider.hpp"

#include "../../Utils/UninitializedObject.hpp"

#include "../../Serialization/GrenadeSerialization.hpp"

#include "glm/ext/vector_float3.hpp"

#include "imgui.h"

#include "octree-cpp/OctreeCpp.h"

#include <cctype>
#include <cstddef>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

struct GameEvent;

class GrenadeHelper : public Feature {
	Checkbox enabled{ this, "Enabled", false };
	FloatSlider render_distance{ this, "Render distance", 0.0F, 32768.0F, 1024.0F };

	FloatSlider aim_circle_radius{ this, "Aim circle radius", 0.0F, 20.0F, 5.0F };
	Color aim_circle_color{ this, "Aim circle color", ImColor{ 1.0F, 1.0F, 1.0F, 1.0F } };
	Color aim_circle_fill_color{ this, "Aim circle fill color", ImColor{ 0.0F, 1.0F, 0.0F, 1.0F } };

	using Grenade = Serialization::Grenades::Grenade;

	struct GrenadeBundle {
		std::vector<Grenade> grenades;
		std::vector<std::pair<std::string, std::size_t>> counts;
		std::size_t hash = 0;
	};

	// TODO: Verify that this is actually the boundaries that the s2 engine restricts to.
	static constexpr float OCTREE_SIZE = 32768;

	using Octree = OctreeCpp<glm::vec3, std::unordered_map<GrenadeType, std::shared_ptr<GrenadeBundle>>>;
	Octree grenades;
	std::string current_map;

	struct ProximateGrenadeBundle {
		std::shared_ptr<GrenadeBundle> grenades;
		float alpha;
		glm::vec3 position;
		bool in_position; // Is the player standing on this bundle?
	};

	std::vector<ProximateGrenadeBundle> proximate_grenades;
	glm::vec3 view_offset;
	glm::vec3 player_viewangles;
	bool ducking;
	mutable std::mutex proximate_grenades_mutex;

	void reset_octree();

	void clear_current_grenades();
	static void draw_surrounded_grenade(const ProximateGrenadeBundle& bundle, ImVec2 screen_pos);
	void draw_aim_helpers(const Grenade& grenade, ImVec2 screen_pos) const;

public:
	GrenadeHelper();

	void update_viewangles(const glm::vec3& viewangles);
	void update();

	void event_handler(GameEvent* event);
	void draw() const;
};

inline UninitializedObject<GrenadeHelper> grenade_helper;
