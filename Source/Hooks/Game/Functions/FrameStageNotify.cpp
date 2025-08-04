#include "../GameHook.hpp"

#include "RetAddrSpoofer.hpp"

#include "../../../Features/Misc/BombTimer.hpp"
#include "../../../Features/Misc/Hitmarker.hpp"
#include "../../../Features/PlayerList/PlayerList.hpp"
#include "../../../Features/Visuals/Chams/Chams.hpp"
#include "../../../Features/Visuals/ESP/ESP.hpp"
#include "../../../Features/Visuals/GrenadeHelper.hpp"
#include "../../../Features/Visuals/GrenadePrediction.hpp"
#include "../../../Features/Visuals/ModelChanger/ModelChanger.hpp"
#include "../../../Features/Visuals/Removals.hpp"

#include "../../../SDK/Entities/CSPlayerController.hpp"
#include "../../../SDK/Enums/ClientFrameStage.hpp"

#include "../../../GUI/GUI.hpp"

#include "../../../Memory.hpp"

#include "../../../Utils/MutexGuard.hpp"

#include "imgui.h"

void Hooks::Game::FrameStageNotify::hookFunc([[maybe_unused]] void* thisptr, ClientFrameStage stage)
{
	switch (stage) {
		using enum ClientFrameStage;
	case FRAME_SIMULATE_END: {
		CSPlayerController* controller = *Memory::local_player_controller;
		Memory::local_player = controller ? controller->player_pawn().get() : nullptr;

		thread_executor.run_all_queued_functions();
		break;
	}

	case FRAME_NET_UPDATE_POSTDATAUPDATE_END: {
		model_changer->update_model();
		break;
	}

	// TODO Ghetto fix, need to find new enum
	case FRAME_NET_UPDATE_END: {
		CSPlayerController* controller = *Memory::local_player_controller;
		Memory::local_player = controller ? controller->player_pawn().get() : nullptr;

		grenade_helper->update();
		chams->update_pvs();
		removals->remove_ads_update();
		bomb_timer->update();
		player_list->update();

		const MutexGuard<ImDrawList*> draw_list_guard = GUI::get_draw_list();
		ImDrawList* draw_list = *draw_list_guard;
		if (draw_list != nullptr) { // it was not yet initialized by the other thread
			draw_list->_ResetForNewFrame();
			draw_list->PushClipRectFullScreen();
			draw_list->PushTextureID(ImGui::GetIO().Fonts->TexID);
			esp->draw(draw_list);
			grenade_prediction->draw(draw_list);
			hit_marker->draw(draw_list);
		}
		break;
	}

	default:
		break;
	}

	RetAddrSpoofer::invoke<void>(reinterpret_cast<void*>(hook->get_trampoline()), thisptr, stage);
}
