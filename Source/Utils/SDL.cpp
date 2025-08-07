#include "SDL.hpp"

#include "Logging.hpp"

#include "SDL3/SDL_version.h"

#include <cstring>
#include <string_view>

void SDL::verify_version()
{
	// In the 28th July update valve decided that git is bloat and thus SDL_REVISION is no longer useful.
	// It just reports "SDL3-3.3.0-HEAD-HASH-NOTFOUND"
	// I'm keeping this in hope that they at some point return to the original behavior,
	// I use the latest release as of right now in the meantime...
	const char* revision = SDL_GetRevision();
	if (std::string_view{ revision } != FW_SDL_REVISION) {
		Logging::error("Wrong SDL version! Expected '{}', got '{}'.", FW_SDL_REVISION, revision);
	}
}
