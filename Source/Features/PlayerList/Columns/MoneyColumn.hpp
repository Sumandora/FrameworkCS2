#pragma once

#include "../Columns.hpp"

#include "../../../SDK/Entities/CSPlayerController.hpp"
#include "../../../SDK/Entities/Services/InGameMoneyServices.hpp"

#include "imgui.h"

struct MoneyColumn {
	static constexpr const char* NAME = "Money";
	static constexpr bool HIDDEN_BY_DEFAULT = true;
	InGameMoneyServices::account_type money = 0;

	void update(CSPlayerController* controller)
	{
		InGameMoneyServices* in_game_money_services = controller->in_game_money_services();
		if (!in_game_money_services)
			return;

		money = in_game_money_services->account();
	}

	void fill() const
	{
		ImGui::Text("%d", money);
	}
};

static_assert(Column<MoneyColumn>);
