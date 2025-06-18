#include "../GameHook.hpp"

#include "../../Graphics/GraphicsHook.hpp"

#include "../../../SDK/Entities/BaseEntity.hpp"

#include "../../../SDK/Enums/ClientFrameStage.hpp"

#include "../../../Memory.hpp"

#include "RetAddrSpoofer.hpp"

#include "../../../Features/Visuals/ESP/ESP.hpp"
#include "../../../Features/Visuals/GrenadePrediction.hpp"

#include <mutex>

void Hooks::Game::FrameStageNotify::hookFunc([[maybe_unused]] void* thisptr, ClientFrameStage stage)
{
	switch (stage) {
		using enum ClientFrameStage;
	case FRAME_SIMULATE_END: {
		Memory::local_player = BaseEntity::getLocalPlayer();
		thread_executor.run_all_queued_functions();
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
		}
		break;
	}

	default:
		break;
	}

	RetAddrSpoofer::invoke<void>(reinterpret_cast<void*>(hook->get_trampoline()), thisptr, stage);
}
