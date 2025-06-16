#include "SDL.hpp"

#include "Logging.hpp"

#include "SDL3/SDL_version.h"

#include <cstring>

void SDL::verify_version()
{
	const char* revision = SDL_GetRevision();
	if (strcmp(revision, FW_SDL_REVISION) != 0) {
		Logging::error("Wrong SDL version! Expected '{}', got '{}'.", FW_SDL_REVISION, revision);
	}
}
