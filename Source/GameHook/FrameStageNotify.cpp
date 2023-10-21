#include "GameHook.hpp"

#include "../Features/Features.hpp"
#include "../GraphicsHook/GraphicsHook.hpp"

#include "RetAddrSpoofer.hpp"

void GameHook::FrameStageNotify::hookFunc([[maybe_unused]] void* thisptr, ClientFrameStage stage)
{
	switch (stage) {
		using enum ClientFrameStage;
	case FRAME_NET_UPDATE_END:
		const std::lock_guard<std::mutex> lock(GraphicsHook::espMutex);
		if (GraphicsHook::espDrawList != nullptr) { // it was not yet initialized by the other thread
			GraphicsHook::espDrawList->_ResetForNewFrame();
			GraphicsHook::espDrawList->PushClipRectFullScreen();
			GraphicsHook::espDrawList->PushTextureID(ImGui::GetIO().Fonts->TexID);
			Features::ESP::drawEsp(GraphicsHook::espDrawList);
		}
		break;
	}
	return RetAddrSpoofer::invoke<void>(hook->proxy, thisptr, stage);
}
