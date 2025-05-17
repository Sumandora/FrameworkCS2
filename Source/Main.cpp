#include <cstdio>
#include <thread>

#include "Interfaces.hpp"

#include "Memory.hpp"

#include "GraphicsHook/GraphicsHook.hpp"

#include "GameHook/GameHook.hpp"

static void initializer()
{
	printf("Hello, world, again!\n");

	Memory::mem_mgr.update();

	Interfaces::getInterfaces();

	Memory::Create();

	if (!GraphicsHook::hookSDL()) {
		printf("Failed to hook SDL\n");
		return;
	}

	if (!GraphicsHook::hookVulkan()) {
		printf("Failed to hook vulkan\n");
		return;
	}

	GameHook::hook();
}

int __attribute((constructor, used)) startup()
{
	printf("Hello, world!\n");
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
