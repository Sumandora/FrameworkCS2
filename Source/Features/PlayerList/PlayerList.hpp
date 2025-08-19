#pragma once

#include "../Feature.hpp"

#include "../Settings/Button.hpp"
#include "../Settings/Checkbox.hpp"

#include "../../Utils/UninitializedObject.hpp"

#include "PlayerListWidget.hpp"

#include <cstddef>
#include <mutex>
#include <string_view>

struct CSPlayerController;
struct PlayerData;
struct GameEvent;

class PlayerList : public Feature {
	PlayerListWidget widget{ this, "Widget", this };
	Checkbox draw_overlay_player_list{ this, "Draw overlay player list", false };
	Button invalidate_caches_button{ this, "Invalidate caches", [this] { invalidate_caches(); } };

	friend PlayerListWidget;

	// @returns amount of entries
	std::size_t render_player_list();
	void invalidate_caches();

	std::mutex mutex;

public:
	PlayerList();

	void update();
	void event_handler(GameEvent* event, std::string_view event_name);
	void draw();
};

inline UninitializedObject<PlayerList> player_list;
