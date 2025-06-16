#include <thread>

#include "Features/Features.hpp"
#include "GUI/GUI.hpp"
#include "Interfaces.hpp"

#include "Memory.hpp"

#include "Hooks/Hooks.hpp"

#include "Serialization/Serialization.hpp"

#include "Utils/Logging.hpp"
#include "Utils/Protobuf.hpp"
#include "Utils/SDL.hpp"

static void initializer()
{
	Logging::debug("Hello, world, from inside the thread!");

	Memory::mem_mgr.sync_layout();

	SDL::verify_version();

	GUI::init();
	Interfaces::grab_interfaces();
	Memory::create();

	Protobuf::verify_version();

	Features::create();
	Serialization::create_config_directory();
	Hooks::create();

	Logging::info("Initialization complete");
}

static int __attribute((constructor)) startup()
{
	Logging::debug("Hello, world!");
	std::thread t(initializer);
	t.detach();

	return 0;
}

static void __attribute((destructor)) shutdown()
{
	Serialization::shutdown();
	Hooks::destroy();
	Features::destroy();
	GUI::destroy();
}
