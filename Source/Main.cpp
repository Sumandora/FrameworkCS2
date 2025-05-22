#include <thread>

#include "GUI/GUI.hpp"
#include "Interfaces.hpp"

#include "Memory.hpp"

#include "Hooks/Hooks.hpp"
#include "Utils/Logging.hpp"

static void initializer()
{
	Logging::info("Hello, world, again!");

	Memory::mem_mgr.update();

	GUI::init();

	Interfaces::getInterfaces();

	Memory::Create();

	Hooks::create();
}

static int __attribute((constructor)) startup()
{
	Logging::info("Hello, world!");
	std::thread t(initializer);
	t.detach();

	return 0;
}

static void __attribute((destructor)) shutdown()
{
	Hooks::destroy();
	GUI::destroy();
}
