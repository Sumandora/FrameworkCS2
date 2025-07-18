#include "../GameHook.hpp"

#include "../../Graphics/GraphicsHook.hpp"

#include "../../../Memory.hpp"

#include "RetAddrSpoofer.hpp"

#include "../../../Features/Misc/Hitmarker.hpp"
#include "../../../Features/Misc/BombTimer.hpp"
#include "../../../Features/Visuals/Chams/Chams.hpp"
#include "../../../Features/Visuals/ESP/ESP.hpp"
#include "../../../Features/Visuals/GrenadeHelper.hpp"
#include "../../../Features/Visuals/GrenadePrediction.hpp"
#include "../../../Features/Visuals/ModelChanger/ModelChanger.hpp"
#include "../../../Features/Visuals/Removals.hpp"

#include "../../../SDK/Entities/CSPlayerController.hpp"
#include "../../../SDK/Enums/ClientFrameStage.hpp"

#include <mutex>

void Hooks::Game::FrameStageNotify::hookFunc([[maybe_unused]] void* thisptr, ClientFrameStage stage)
{
	switch (stage) {
		using enum ClientFrameStage;
	case FRAME_SIMULATE_END: {
		CSPlayerController* controller = *Memory::local_player_controller;
		Memory::local_player = controller ? controller->player_pawn().get() : nullptr;

		grenade_helper->update();
		chams->update_pvs();
		removals->remove_ads_update();
		bomb_timer->update();

		thread_executor.run_all_queued_functions();
		break;
	}

	case FRAME_NET_UPDATE_POSTDATAUPDATE_END: {
		model_changer->update_model();
		break;
	}

	case FRAME_NET_UPDATE_END: {
		const std::lock_guard<std::mutex> lock(GraphicsHook::espMutex);
		if (GraphicsHook::espDrawList != nullptr) { // it was not yet initialized by the other thread
			GraphicsHook::espDrawList->_ResetForNewFrame();
			GraphicsHook::espDrawList->PushClipRectFullScreen();
			GraphicsHook::espDrawList->PushTextureID(ImGui::GetIO().Fonts->TexID);
			esp->draw(GraphicsHook::espDrawList.get());
			grenade_prediction->draw(GraphicsHook::espDrawList.get());
			hit_marker->draw(GraphicsHook::espDrawList.get());
		}
		break;
	}

	default:
		break;
	}

	RetAddrSpoofer::invoke<void>(reinterpret_cast<void*>(hook->get_trampoline()), thisptr, stage);
}
