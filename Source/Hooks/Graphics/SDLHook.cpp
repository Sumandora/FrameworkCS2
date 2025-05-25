#include "GraphicsHook.hpp"

#include "../../GUI/GUI.hpp"
#include "../../Memory.hpp"
#include "../../Utils/UninitializedObject.hpp"

#include "BCRL/SearchConstraints.hpp"
#include "BCRL/Session.hpp"

#include "RetAddrSpoofer.hpp"

#include "SDL3/SDL_events.h"
#include "SDL3/SDL_stdinc.h"

#include "SignatureScanner/PatternSignature.hpp"
#include "SignatureScanner/XRefSignature.hpp"

#include "backends/imgui_impl_sdl3.h"
#include "imgui.h"

#include "SDL3/SDL_video.h"

#include <memory>
#include <vector>

#include "../Hooks.hpp"

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

static UninitializedObject<Hooks::DetourHook<true>> hook;

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
		[[gnu::aligned(16)]] std::vector<SDL_Event> filtered_events;
		// This is wrong, it might be less, but I rather allocate a bit too much than doing a ton of allocations.
		filtered_events.reserve(numevents);

		for (int i = 0; i < numevents; ++i) {
			const SDL_Event& event = events[i];

			bool swallowed = false;

			if (!ImGui::GetIO().BackendPlatformUserData) {
				if (event.type < SDL_EVENT_WINDOW_FIRST || event.type > SDL_EVENT_WINDOW_LAST)
					// NOLINTNEXTLINE(hicpp-avoid-goto, cppcoreguidelines-avoid-goto)
					goto next_event;

				SDL_Window* window = SDL_GetWindowFromID(event.window.windowID);

				ImGui_ImplSDL3_InitForVulkan(window);
				GUI::provide_window(window);
			} else {
				swallowed = GUI::queue_event(&event);
			}

		next_event:
			if (!swallowed) {
				filtered_events.emplace_back(event);
			}
		}

		return RetAddrSpoofer::invoke<int, SDL_Event*, int, SDL_EventAction, Uint32, Uint32, bool>(
			reinterpret_cast<void*>(hook->get_trampoline()), filtered_events.data(), static_cast<int>(filtered_events.size()), action, minType, maxType, include_sentinel);
	}
	return RetAddrSpoofer::invoke<int, SDL_Event*, int, SDL_EventAction, Uint32, Uint32, bool>(
		reinterpret_cast<void*>(hook->get_trampoline()), events, numevents, action, minType, maxType, include_sentinel);
}

bool GraphicsHook::hookSDL()
{
	void* peep_events_internal_ptr = BCRL::signature(Memory::mem_mgr, SignatureScanner::PatternSignature::for_literal_string<"Event queue is full (%d events)">(), BCRL::everything(Memory::mem_mgr).thats_readable().with_name("libSDL3.so.0"))
										 .find_xrefs(SignatureScanner::XRefTypes::relative(), BCRL::everything(Memory::mem_mgr).thats_readable().with_name("libSDL3.so.0"))
										 .prev_signature_occurrence(SignatureScanner::PatternSignature::for_array_of_bytes<"41 57 41 89 f7">())
										 .expect<void*>("Failed to find backend function pointer for SDL_PeepEventsInternal");

	hook.emplace(Memory::emalloc, peep_events_internal_ptr, reinterpret_cast<void*>(peep_events_hook));

	hook->enable();
	return true;
}

void GraphicsHook::unhookSDL()
{
	hook.reset();

	const ImGuiIO& io = ImGui::GetIO();

	if (io.BackendPlatformUserData) {
		ImGui_ImplSDL3_Shutdown();
	}
}
