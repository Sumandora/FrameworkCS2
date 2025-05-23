#include "GraphicsHook.hpp"

#include "../../GUI/GUI.hpp"
#include "../../Memory.hpp"

#include "BCRL/Session.hpp"

#include "RetAddrSpoofer.hpp"

#include "backends/imgui_impl_sdl3.h"
#include "imgui.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_stdinc.h"

#include "SDL3/SDL_video.h"

#include <memory>
#include <vector>

#include "../Game/GameHook.hpp"

// int SDL_PeepEventsInternal(
// 		SDL_Event *events,
// 		int numevents,
// 		SDL_EventAction action,
// 		Uint32 minType,
// 		Uint32 maxType,
// 		bool include_sentinel);

// NOLINTBEGIN(readability-identifier-naming)
using SDL_PeepEventsInternal = int (*)(
	SDL_Event* events,
	int numevents,
	SDL_EventAction action,
	Uint32 minType,
	Uint32 maxType,
	bool include_sentinel);
// NOLINTEND(readability-identifier-naming)

static std::unique_ptr<Hooks::Game::GameHook> hook;

static int peep_events_hook(
	// NOLINTBEGIN(readability-identifier-naming)
	SDL_Event* events,
	int numevents,
	SDL_EventAction action,
	Uint32 minType,
	Uint32 maxType,
	bool include_sentinel
	// NOLINTEND(readability-identifier-naming)
)
{
	if (action == SDL_ADDEVENT && numevents > 0) {
		std::vector<SDL_Event> filtered_events;
		// This is wrong, it might be less, but I rather allocate a bit too much than doing a ton of allocations.
		filtered_events.reserve(numevents);

		for (int i = 0; i < numevents; ++i) {
			const SDL_Event& event = events[i];

			bool swallowed = false;

			if (!ImGui::GetIO().BackendPlatformUserData) {
				if (event.type < SDL_EVENT_WINDOW_FIRST || event.type > SDL_EVENT_WINDOW_LAST)
					continue;

				SDL_Window* window = SDL_GetWindowFromID(event.window.windowID);

				ImGui_ImplSDL3_InitForVulkan(window);

				GUI::create_font(window);
			} else {
				swallowed = GUI::queue_event(&event);
			}

			if (!swallowed) {
				filtered_events.push_back(event);
			}
		}

		return RetAddrSpoofer::invoke<int, SDL_Event*, int, SDL_EventAction, Uint32, Uint32, bool>(
			hook->get_proxy(), filtered_events.data(), static_cast<int>(filtered_events.size()), action, minType, maxType, include_sentinel);
	}
	return RetAddrSpoofer::invoke<int, SDL_Event*, int, SDL_EventAction, Uint32, Uint32, bool>(
		hook->get_proxy(), events, numevents, action, minType, maxType, include_sentinel);
}

bool GraphicsHook::hookSDL()
{
	// TODO Find better signature
	void* peep_events_internal_ptr = BCRL::signature(Memory::mem_mgr, SignatureScanner::PatternSignature::for_array_of_bytes<"41 89 F8 31 F6">(), BCRL::everything(Memory::mem_mgr).thats_readable().with_name("libSDL3.so.0"))
										 .add(13)
										 .relative_to_absolute()
										 .expect<void*>("Failed to find backend function pointer for SDL_PeepEvents");

	hook = std::make_unique<Hooks::Game::GameHook>(peep_events_internal_ptr, reinterpret_cast<void*>(peep_events_hook));
	return true;
}

void GraphicsHook::unhookSDL()
{
	hook = nullptr;

	const ImGuiIO& io = ImGui::GetIO();

	if (io.BackendPlatformUserData) {
		ImGui_ImplSDL3_Shutdown();
	}
}
