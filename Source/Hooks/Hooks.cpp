#include "Hooks.hpp"

#include "Game/GameHook.hpp"
#include "Graphics/GraphicsHook.hpp"

#include "../Utils/Logging.hpp"

void Hooks::create()
{
	if (!GraphicsHook::hookSDL()) {
		Logging::error("Failed to hook SDL");
		return;
	}

	if (!GraphicsHook::hookVulkan()) {
		Logging::error("Failed to hook vulkan");
		return;
	}

	Game::create();
}

void Hooks::destroy()
{
	GraphicsHook::unhookVulkan();
	GraphicsHook::unhookSDL();

	Game::destroy();
}
