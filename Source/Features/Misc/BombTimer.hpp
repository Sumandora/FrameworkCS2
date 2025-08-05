#pragma once

#include "../Feature.hpp"

#include "../Settings/Checkbox.hpp"

#include "../../Utils/UninitializedObject.hpp"

#include "../../SDK/Entities/MapInfo.hpp"
#include "../../SDK/Entities/PlantedC4.hpp"
#include "../../SDK/EntityHandle.hpp"

#include <cstdint>
#include <mutex>
#include <optional>
#include <string>

class BombTimer : public Feature {
	Checkbox enabled{ this, "Enabled", false };

	EntityHandle<PlantedC4> planted_c4 = EntityHandle<PlantedC4>::invalid();
	PlantedC4* update_planted_c4();

	EntityHandle<MapInfo> map_info = EntityHandle<MapInfo>::invalid();
	MapInfo* update_map_info();

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

	void clear_bomb_info();

public:
	BombTimer();

	void update();
	void draw();
};

inline UninitializedObject<BombTimer> bomb_timer;
