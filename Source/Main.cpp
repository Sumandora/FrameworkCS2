#include <cstdio>
#include <thread>

#include "Interfaces.hpp"

#include "Memory.hpp"

#include "Hooks/Hooks.hpp"
#include "Utils/Logging.hpp"

static void initializer()
{
	Logging::info("Hello, world, again!");

	Memory::mem_mgr.update();

	Interfaces::getInterfaces();

	Memory::Create();

	Hooks::create();
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
	Hooks::destroy();
}
