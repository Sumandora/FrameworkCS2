#pragma once

#include "../Feature.hpp"

#include "../Settings/Checkbox.hpp"

#include "../../Utils/UninitializedObject.hpp"

#include "../../SDK/Entities/PlantedC4.hpp"
#include "../../SDK/Entities/MapInfo.hpp"
#include "../../SDK/EntityHandle.hpp"

#include <cstdint>
#include <mutex>
#include <optional>
#include <string>

class BombTimer : public Feature {
	Checkbox enabled{ this, "Enabled", false };

	EntityHandle<PlantedC4> planted_c4 = EntityHandle<PlantedC4>::invalid();
	float bomb_radius = 500.0F;

	struct BombInfo {
		float current_time;
		float explode_time;
		float damage;
		std::uint32_t site;

		struct DefuseInfo {
			std::string defuser_name;
			float end_time;
		};

		std::optional<DefuseInfo> defuse_info;
	};

	std::optional<BombInfo> bomb_info;
	std::mutex bomb_info_lock;

public:
	BombTimer();

	void update();
	void draw();

	void update_bomb_radius(float radius);
};

inline UninitializedObject<BombTimer> bomb_timer;
