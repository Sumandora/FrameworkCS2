#include "GraphicsHook.hpp"
#include <mutex>
#include <thread>

#include <iostream>
#include <SDL3/SDL.h>

#include "BCRL.hpp"

#include "backends/imgui_impl_sdl3.h"

#include "../Menu/Menu.hpp"

using SDL_PeepEventsFunc = int (*)(SDL_Event* events, int numevents, SDL_eventaction action, Uint32 minType, Uint32 maxType);
static SDL_PeepEventsFunc originalPeepEvents;
static SDL_PeepEventsFunc* functionPtr;

static int peepEventsHook(SDL_Event* events, int numevents, SDL_eventaction action, Uint32 minType, Uint32 maxType)
{
	int rv = originalPeepEvents(events, numevents, action, minType, maxType);
	if (rv == 0)
		return rv;

	for (int i = 0; i < numevents; ++i) {
		SDL_Event& event = events[i];

		if (!ImGui::GetCurrentContext()) {
			if (event.type < SDL_EVENT_WINDOW_FIRST || event.type > SDL_EVENT_WINDOW_LAST)
				continue;

			ImGui::CreateContext();

			ImGui_ImplSDL3_InitForVulkan(SDL_GetWindowFromID(event.window.windowID));

			ImGuiIO& io = ImGui::GetIO();

			io.IniFilename = nullptr; // TODO Bring it back
			io.LogFilename = nullptr;

			Menu::init();
			printf("Initialized ImGui Context\n");
		} else {
			std::lock_guard<std::mutex> lock{ GraphicsHook::eventAccessMutex };
			GraphicsHook::eventQueue.emplace_back(event);
			// TODO Discard events in case the menu is open
		}
	}

	return rv;
}

bool GraphicsHook::hookSDL() // TODO Write something that works universally on SDL
{
	functionPtr = static_cast<SDL_PeepEventsFunc*>(
		BCRL::Session::pointer(reinterpret_cast<void*>(SDL_PeepEvents))
			.add(2)
			.relativeToAbsolute()
			.expect("Failed to find backend function pointer for SDL_PeepEvents"));

	originalPeepEvents = *functionPtr;
	*functionPtr = peepEventsHook;
	return true;
}

void GraphicsHook::unhookSDL()
{
	*functionPtr = originalPeepEvents;
}