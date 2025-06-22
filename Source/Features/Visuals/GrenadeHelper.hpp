#pragma once

#include "../Feature.hpp"

#include "../Settings/Checkbox.hpp"
#include "../Settings/FloatSlider.hpp"

#include "../../Utils/UninitializedObject.hpp"

#include "../../Serialization/GrenadeSerialization.hpp"

#include "glm/ext/vector_float3.hpp"

#include "imgui.h"

#include "octree-cpp/OctreeCpp.h"

#include <atomic>
#include <cctype>
#include <cstddef>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

struct GameEvent;

class GrenadeHelper : public Feature {

	Checkbox enabled{ this, "Enabled", false };
	FloatSlider render_distance{ this, "Render distance", 0.0F, 32768.0F, 1024.0F };

	using GrenadeWeapon = Serialization::Grenades::GrenadeWeapon;
	using Grenade = Serialization::Grenades::Grenade;

	using Octree = OctreeCpp<glm::vec3, std::unordered_map<GrenadeWeapon, std::vector<Grenade>>>;
	Octree grenades;
	std::string current_map;

	struct GrenadeBundle {
		std::vector<Grenade> grenades;
		float alpha;
		glm::vec3 position;
		std::size_t hash;
		bool in_position; // Is the player standing on this bundle?
	};

	std::vector<GrenadeBundle> proximate_grenades;
	glm::vec3 view_offset;
	std::atomic<glm::vec3> player_viewangles;
	mutable std::mutex proximate_grenades_mutex;

	void clear_current_grenades();
	static void draw_surrounded_grenade(const GrenadeBundle& bundle, ImVec2 screen_pos);
	void draw_aim_helpers(const Grenade& grenade, ImVec2 screen_pos) const;

public:
	GrenadeHelper();

	void update_viewangles(const glm::vec3& viewangles);
	void update();

	void event_handler(GameEvent* event);
	void draw() const;
};

inline UninitializedObject<GrenadeHelper> grenade_helper;
