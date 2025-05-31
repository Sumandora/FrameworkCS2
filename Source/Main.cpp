#include <thread>

#include "Features/Features.hpp"
#include "GUI/GUI.hpp"
#include "Interfaces.hpp"

#include "Memory.hpp"

#include "Hooks/Hooks.hpp"
#include "Utils/Logging.hpp"

static void initializer()
{
	Logging::info("Hello, world, from inside the thread!");

	Memory::mem_mgr.sync_layout();

	GUI::init();
	Interfaces::grab_interfaces();
	Memory::create();
	Features::create();
	Hooks::create();

	Logging::info("Initialization complete");
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
	Features::destroy();
	GUI::destroy();
}
