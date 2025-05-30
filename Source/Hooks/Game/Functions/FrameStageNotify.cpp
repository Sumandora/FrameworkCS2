#include "../GameHook.hpp"

#include "../../Graphics/GraphicsHook.hpp"

#include "../../../Features/Features.hpp"

#include "../../../SDK/Entities/BaseEntity.hpp"
#include "../../../SDK/GameClass/ClientFrameStage.hpp"

#include "../../../Memory.hpp"

#include "RetAddrSpoofer.hpp"

#include "backends/imgui_impl_vulkan.h"

#include <mutex>

void Hooks::Game::FrameStageNotify::hookFunc([[maybe_unused]] void* thisptr, ClientFrameStage stage)
{
	switch (stage) {
		using enum ClientFrameStage;
	case FRAME_SIMULATE_END: {
		Memory::local_player = BaseEntity::getLocalPlayer();
		break;
	}

	case FRAME_NET_UPDATE_END: {
		const std::lock_guard<std::mutex> lock(GraphicsHook::espMutex);
		if (GraphicsHook::espDrawList != nullptr) { // it was not yet initialized by the other thread
			GraphicsHook::espDrawList->_ResetForNewFrame();
			GraphicsHook::espDrawList->PushClipRectFullScreen();
			GraphicsHook::espDrawList->PushTextureID(ImGui::GetIO().Fonts->TexID);
			Features::esp->draw(GraphicsHook::espDrawList.get());
		}
		break;
	}

	default:
		break;
	}

	RetAddrSpoofer::invoke<void>(reinterpret_cast<void*>(hook->get_trampoline()), thisptr, stage);
}
