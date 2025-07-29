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
	SDL_Event* event
	// NOLINTEND(readability-identifier-naming)
)
{
	bool swallowed = false;

	if (!ImGui::GetIO().BackendPlatformUserData) {
		if (event->type < SDL_EVENT_WINDOW_FIRST || event->type > SDL_EVENT_WINDOW_LAST)
			return RetAddrSpoofer::invoke<int>(reinterpret_cast<void*>(hook->get_trampoline()), event);

		SDL_Window* window = SDL_GetWindowFromID(event->window.windowID);

		ImGui_ImplSDL3_InitForVulkan(window);
		GUI::provide_window(window);
	} else {
		swallowed = GUI::queue_event(event);
	}

	if (swallowed) {
		return 1;
	}

	return RetAddrSpoofer::invoke<int>(reinterpret_cast<void*>(hook->get_trampoline()), event);
}

bool GraphicsHook::hookSDL()
{
	// void* peep_events_internal_ptr
	// 	= BCRL::signature(
	// 		Memory::mem_mgr,
	// 		SignatureScanner::PatternSignature::for_literal_string<"Event queue is full (%d events)">(),
	// 		BCRL::everything(Memory::mem_mgr).thats_readable().with_name("libSDL3.so.0"))
	// 		  .find_xrefs(
	// 			  SignatureScanner::XRefTypes::relative(),
	// 			  BCRL::everything(Memory::mem_mgr).thats_readable().with_name("libSDL3.so.0"))
	// 		  .prev_signature_occurrence(SignatureScanner::PatternSignature::for_array_of_bytes<"55 48 89 fd">())
	// 		  .expect<void*>("Failed to find backend function pointer for SDL_PeepEvents");
	void* peep_events_internal_ptr
		= BCRL::signature(
			Memory::mem_mgr,
			SignatureScanner::PatternSignature::for_array_of_bytes<"41 54 55 48 89 FD 48 83 EC 08 48 83 7F ? 00">(),
			BCRL::everything(Memory::mem_mgr).thats_readable().with_name("libSDL3.so.0"))
			  .expect<void*>("Failed to find backend function pointer for SDL_PeepEvents");

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
