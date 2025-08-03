#include "PlayerListWidget.hpp"

#include "PlayerList.hpp"

#include "../Setting.hpp"

#include <utility>

PlayerListWidget::PlayerListWidget(SettingsHolder* parent, std::string name, PlayerList* player_list)
	: Setting(parent, std::move(name))
	, player_list(player_list)
{
}

void PlayerListWidget::render()
{
	player_list->render_player_list();
}
