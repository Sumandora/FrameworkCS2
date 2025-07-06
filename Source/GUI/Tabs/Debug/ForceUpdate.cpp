#include "../Tabs.hpp"

#include "../../../Hooks/Game/GameHook.hpp"

#include "../../../Interfaces.hpp"

#include "../../../SDK/GameClass/NetworkClientService.hpp"
#include "../../../SDK/GameClass/NetworkGameClient.hpp"

#include "imgui.h"

void GUI::Tabs::Debug::draw_force_update()
{
	if (ImGui::Button("Force full update")) {
		Hooks::Game::FrameStageNotify::thread_executor.queue([] {
			NetworkClientService* network_client_service = Interfaces::network_client_service;
			if (!network_client_service)
				return;
			NetworkGameClient* network_game_client = network_client_service->network_game_client;
			if (!network_game_client)
				return;

			// Reason doesn't seem to propagate any further down. It is only used to print a message.
			// This is the same reason as the cl_fullupdate command.
			network_game_client->force_full_update("forced by console");
		});
	}

	ImGui::TextWrapped("WARNING: Full updates can cause massive glitches and bugs, especially if they are executed in quick succession. "
					   "Try to avoid spamming the button too often. Nobody is preventing you from shooting yourself in the foot. "
					   "Also remember, that this is detectable, as you are literally requesting a full update packet with no apparant reason.");
}
