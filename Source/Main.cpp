#include "Features/Features.hpp"
#include "GUI/GUI.hpp"
#include "Hooks/Hooks.hpp"
#include "Interfaces.hpp"
#include "Libraries.hpp"
#include "Memory.hpp"
#include "Serialization/Serialization.hpp"
#include "Utils/Logging.hpp"
#include "Utils/Protobuf.hpp"
#include "Utils/SDL.hpp"

#include <thread>

static void initializer()
{
	Logging::debug("Hello, world, from inside the thread!");

	Memory::mem_mgr.sync_layout();

	SDL::verify_version();

	Serialization::create_config_directory();
	GUI::init(Serialization::get_config_directory());

	Libraries::create();
	Interfaces::grab_interfaces();
	Memory::create();

	Protobuf::verify_version();

	Features::create();
	Hooks::create();

	Libraries::destroy();
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
