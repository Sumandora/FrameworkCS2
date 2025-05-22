#include "GraphicsHook.hpp"

#include "../../GUI/GUI.hpp"
#include "../../Memory.hpp"
#include "../../Utils/Logging.hpp"

#include "BCRL/Session.hpp"

#include "backends/imgui_impl_sdl3.h"
#include "imgui.h"

#include "SDL3/SDL_video.h"

#include <cstdint>
#include <mutex>

using SDL_PeepEventsFunc = int (*)(SDL_Event* events, int numevents, SDL_EventAction action, Uint32 minType, Uint32 maxType);
static SDL_PeepEventsFunc originalPeepEvents;
static SDL_PeepEventsFunc* functionPtr;

static int peepEventsHook(SDL_Event* events, int numevents, SDL_EventAction action, Uint32 minType, Uint32 maxType)
{
	int rv = originalPeepEvents(events, numevents, action, minType, maxType);
	if (rv == 0)
		return rv;

	for (int i = 0; i < numevents; ++i) {
		const SDL_Event& event = events[i];

		if (!ImGui::GetIO().BackendPlatformUserData) {
			if (event.type < SDL_EVENT_WINDOW_FIRST || event.type > SDL_EVENT_WINDOW_LAST)
				continue;

			ImGui_ImplSDL3_InitForVulkan(SDL_GetWindowFromID(event.window.windowID));
		} else {
			GUI::queue_event(&event);
			// TODO Discard events in case the menu is open
		}
	}

	return rv;
}

bool GraphicsHook::hookSDL() // TODO Write something that works universally on SDL
{
	functionPtr = BCRL::pointer(Memory::mem_mgr, reinterpret_cast<std::uintptr_t>(SDL_PeepEvents))
					  .add(2)
					  .relative_to_absolute()
					  .expect<SDL_PeepEventsFunc*>("Failed to find backend function pointer for SDL_PeepEvents");

	originalPeepEvents = *functionPtr;
	*functionPtr = peepEventsHook;
	return true;
}

void GraphicsHook::unhookSDL()
{
	*functionPtr = originalPeepEvents;

	const ImGuiIO& io = ImGui::GetIO();

	if (io.BackendPlatformUserData) {
		ImGui_ImplSDL3_Shutdown();
	}
}
