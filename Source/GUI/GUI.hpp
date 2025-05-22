#pragma once

#include "SDL3/SDL_events.h"
#include "SDL3/SDL_video.h"

namespace GUI {
	void init();
	void destroy();

	void create_font(SDL_Window* window);

	void render();

	void queue_event(const SDL_Event* event);
	void flush_events();
}
