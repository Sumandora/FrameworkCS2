#include "Player.hpp"

#include "ESP.hpp"

#include "../../../SDK/Entities/CSPlayerController.hpp"
#include "../../../SDK/Entities/CSPlayerPawn.hpp"
#include "../../../SDK/EntityHandle.hpp"

#include "GenericESP/UnionedRect.hpp"

#include "imgui.h"
#include "imgui_internal.h"

#include "Skeleton.hpp"

bool ESPPlayer::is_enabled() const
{
	if (this == &esp->local) {
		// TODO If we are in first person then never say yes here.
	}
	return box_enabled.get() || name_enabled.get() || healthbar_enabled.get() || skeleton_enabled.get();
}

void ESPPlayer::draw_player(ImDrawList* draw_list, CSPlayerPawn* player_pawn, const ImRect& screenspace_rect) const
{
	GenericESP::UnionedRect unioned_rect{ screenspace_rect };

	if (box_enabled.get())
		box.draw(draw_list, player_pawn, unioned_rect);

	if (name_enabled.get()) {
		CSPlayerController* controller = player_pawn->original_controller().get();
		if (controller)
			name.draw(draw_list, player_pawn, controller->get_decorated_player_name(), unioned_rect);
	}

	if (healthbar_enabled.get())
		healthbar.draw(draw_list, player_pawn, unioned_rect);

	if (this->skeleton_enabled.get())
		draw_skeleton(player_pawn, draw_list, skeleton_line);
}
