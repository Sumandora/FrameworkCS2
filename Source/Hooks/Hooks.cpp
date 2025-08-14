#include "Hooks.hpp"

#include "Game/GameHook.hpp"
#include "Graphics/GraphicsHook.hpp"

#include "../Utils/Logging.hpp"

#include <csignal>
#include <sys/types.h>
#include <unistd.h>

void Hooks::create()
{
	Game::create();

	if (!GraphicsHook::hookSDL()) {
		Logging::error("Failed to hook SDL");
		return;
	}

	if (!GraphicsHook::hookVulkan()) {
		Logging::error("Failed to hook vulkan");
		return;
	}
}

void Hooks::destroy()
{
	GraphicsHook::unhookVulkan();
	GraphicsHook::unhookSDL();

	Game::destroy();
}
