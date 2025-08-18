#pragma once

#include "../Columns.hpp"

#include "../../../SDK/Entities/BaseEntity.hpp"
#include "../../../SDK/Entities/BasePlayerPawn.hpp"
#include "../../../SDK/Entities/CSPlayerController.hpp"
#include "../../../SDK/Entities/CSPlayerPawnBase.hpp"
#include "../../../SDK/Entities/Services/PlayerObserverServices.hpp"
#include "../../../SDK/EntityHandle.hpp"

#include "imgui.h"

#include <string>

struct ObserveeColumn {
	static constexpr const char* NAME = "Observee";
	static constexpr bool HIDDEN_BY_DEFAULT = true;
	std::string name;
	bool enabled = false;

	void update(CSPlayerController* controller)
	{
		if (!enabled) {
			name.clear();
			return;
		}

		const BasePlayerPawn* pawn = controller->pawn().get();
		if (!pawn) {
			name.clear();
			return;
		}

		const PlayerObserverServices* observer_services = pawn->observer_services();
		if (!observer_services) {
			name.clear();
			return;
		}

		BaseEntity* observed_ent = observer_services->observer_target().get();
		if (!observed_ent) {
			name.clear();
			return;
		}

		const auto* player_pawn_base = observed_ent->entity_cast<CSPlayerPawnBase*>();
		if (!player_pawn_base) {
			name.clear();
			return;
		}

		const CSPlayerController* original_controller = player_pawn_base->original_controller().get();
		if (!original_controller) {
			name.clear();
			return;
		}

		name = original_controller->get_decorated_player_name();
	}

	void fill() const
	{
		if (!name.empty())
			ImGui::Text("%s", name.c_str());
	}
};

static_assert(Column<ObserveeColumn>);
