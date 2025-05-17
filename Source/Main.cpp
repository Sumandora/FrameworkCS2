#include <cstdio>
#include <thread>

#include "Interfaces.hpp"

#include "Memory.hpp"

#include "GraphicsHook/GraphicsHook.hpp"

#include "GameHook/GameHook.hpp"
#include "Utils/Logging.hpp"

static void initializer()
{
	Logging::info("Hello, world, again!");

	Memory::mem_mgr.update();

	Interfaces::getInterfaces();

	Memory::Create();

	if (!GraphicsHook::hookSDL()) {
		Logging::error("Failed to hook SDL");
		return;
	}

	if (!GraphicsHook::hookVulkan()) {
		Logging::error("Failed to hook vulkan");
		return;
	}

	GameHook::hook();
}

int __attribute((constructor, used)) startup()
{
	Logging::info("Hello, world!");
	std::thread t(initializer);
	t.detach();

	return 0;
}

void __attribute((destructor)) shutdown()
{
	GraphicsHook::unhookSDL();
	GraphicsHook::unhookVulkan();
	GameHook::unhook();
}
