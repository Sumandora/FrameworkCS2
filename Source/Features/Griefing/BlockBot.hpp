#pragma once

#include "../Feature.hpp"

#include "../Settings/Checkbox.hpp"
#include "../Settings/Color.hpp"
#include "../Settings/FloatSlider.hpp"
#include "../Settings/HelpMarker.hpp"

#include "../../SDK/EntityHandle.hpp"

#include "../../Utils/UninitializedObject.hpp"

#include "glm/ext/vector_float3.hpp"

#include "imgui.h"

#include <atomic>
#include <mutex>
#include <optional>
#include <string>

struct CSPlayerPawn;
struct UserCmd;
struct GameSceneNode;

class BlockBot : public Feature {
	Checkbox enabled{ this, "Enabled", false };
	Checkbox round_direction{ this, "Round direction", true };
	HelpMarker round_direction_explanation{ this, "Rounds direction to closest 45° angle." };

	// credits to @michtarxd for those values:
	FloatSlider local_extrapolation_ticks{ this, "Local interpolation ticks", 0.0F, 10.0F, 4.0F };
	FloatSlider target_extrapolation_ticks{ this, "Target interpolation ticks", 0.0F, 10.0F, 2.0F };
	Checkbox add_ping_to_extrapolation_ticks{ this, "Add ping to extrapolation ticks", false };

	Checkbox slow_walk_when_close_to_target{ this, "Slow walk when close to target", false };
	FloatSlider slow_walk_threshold{ this, "Slow walk threshold", 0.0F, 100.0F, 30.0F };

	Color direction_line_color{ this, "Direction line color", ImColor{ 1.0F, 1.0F, 1.0F, 1.0F } };
	Color target_color{ this, "Target color", ImColor{ 1.0F, 0.0F, 0.0F, 1.0F } };

	EntityHandle<CSPlayerPawn> last_target = EntityHandle<CSPlayerPawn>::invalid();
	glm::vec3 last_direction_vector;
	float last_perpendicular_length;
	struct Visuals {
		glm::vec3 from;
		glm::vec3 local_pos;
		glm::vec3 direction_vector;
		glm::vec3 movement_target;
	};
	std::optional<Visuals> visuals;
	mutable std::mutex visuals_mutex;

	static CSPlayerPawn* find_new_target(GameSceneNode* local_game_scene_node);
	glm::vec3 calculate_direction_vector(glm::vec3 local_pos, glm::vec3 other_pos);

	void reset();

public:
	BlockBot();

	void create_move(UserCmd* cmd);
	void draw(ImDrawList* draw_list);

	bool wants_silent_aim() const;
};

inline UninitializedObject<BlockBot> block_bot;
