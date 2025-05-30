#pragma once

#include "SDL3/SDL_events.h"
#include "SDL3/SDL_video.h"

namespace GUI {
	void init();
	void destroy();

	void provide_window(SDL_Window* window);

	void render();

	/**
	 * @returns if the event should be swallowed
	 */
	bool queue_event(const SDL_Event* event);
	void flush_events();

	bool is_using_wayland();

	float get_scale();
	float get_base_font_size();
}
