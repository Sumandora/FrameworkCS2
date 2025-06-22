#pragma once

#include "../Feature.hpp"

#include "../Settings/Checkbox.hpp"
#include "../Settings/FloatSlider.hpp"

#include "../../Utils/UninitializedObject.hpp"

#include "../../Serialization/GrenadeSerialization.hpp"

#include "glm/ext/vector_float3.hpp"

#include "octree-cpp/OctreeCpp.h"

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
	};

	std::vector<GrenadeBundle> proximate_grenades;
	mutable std::mutex proximate_grenades_mutex;

public:
	GrenadeHelper();

	void update();

	void event_handler(GameEvent* event);
	void draw() const;
};

inline UninitializedObject<GrenadeHelper> grenade_helper;
