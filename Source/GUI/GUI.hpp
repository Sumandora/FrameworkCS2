#pragma once

#include "SDL3/SDL_events.h"

namespace GUI {
	void init();
	void destroy();

	void render();

	void queue_event(const SDL_Event* event);
	void flush_events();
}
