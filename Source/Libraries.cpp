#include "Libraries.hpp"
#include "Utils/Logging.hpp"

#include <dlfcn.h>
#include <format>
#include <stdexcept>
#include <string>

using namespace Libraries;

Library::Library(const std::string& name)
	: library_name(name)
	, handle(dlmopen(LM_ID_BASE, name.c_str(), RTLD_NOW | RTLD_NOLOAD | RTLD_LOCAL))
{
	if (!handle)
		throw std::runtime_error{ std::format("Failed to find library '{}'", name) };

	Logging::debug("Library '{}' has handle '{}'", library_name, handle);
}

Library::~Library()
{
	dlclose(handle);
	Logging::debug("Closed handle to library '{}'", library_name);
	handle = nullptr;
}

void* Library::get_symbol(const std::string& name) const
{
	return dlsym(handle, name.c_str());
}

void Libraries::create()
{
	Logging::info("Opening handles to all libraries");

	client.emplace("libclient.so");
	schemasystem.emplace("libschemasystem.so");
	tier0.emplace("libtier0.so");
	engine2.emplace("libengine2.so");
	panorama.emplace("libpanorama.so");
	materialsystem2.emplace("libmaterialsystem2.so");
	resourcesystem.emplace("libresourcesystem.so");
	filesystem.emplace("libfilesystem_stdio.so");
	localize.emplace("liblocalize.so");
	particles.emplace("libparticles.so");
}

void Libraries::destroy()
{
	particles.reset();
	localize.reset();
	filesystem.reset();
	resourcesystem.reset();
	materialsystem2.reset();
	panorama.reset();
	engine2.reset();
	tier0.reset();
	schemasystem.reset();
	client.reset();

	Logging::info("Closed all library handles");
}
