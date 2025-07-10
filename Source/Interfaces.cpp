#include "Interfaces.hpp"

#include "SDK/InterfaceReg.hpp"

#include "BCRL/SearchConstraints.hpp"
#include "BCRL/Session.hpp"

#include "SignatureScanner/PatternSignature.hpp"

#include "Memory.hpp"

#include "Utils/Logging.hpp"

#include <cstdint>
#include <cstring>
#include <dlfcn.h>
#include <expected>
#include <format>
#include <optional>
#include <string_view>
#include <unordered_map>
#include <utility>

using namespace Interfaces;

InterfacedLibrary::InterfacedLibrary(std::unordered_map<const char*, void*> interfaces)
	: interfaces(std::move(interfaces))
{
}

std::optional<InterfacedLibrary> InterfacedLibrary::create(const char* path)
{
	void* handle = dlmopen(LM_ID_BASE, path, RTLD_NOW | RTLD_NOLOAD | RTLD_LOCAL);
	if (!handle)
		return std::nullopt;

	void* create_interface_fn = dlsym(handle, "CreateInterface");
	if (!create_interface_fn)
		return std::nullopt;

	std::expected<InterfaceReg**, BCRL::FinalizationError>
		interface_list = BCRL::pointer(Memory::mem_mgr, create_interface_fn)
							 .add(1)
							 .relative_to_absolute()
							 .filter(BCRL::everything(Memory::mem_mgr).thats_readable().thats_executable().also([le_path = path](const decltype(Memory::mem_mgr)::RegionT& r) {
								 auto path = r.get_path();
								 if (!path.has_value())
									 return false;
								 return path->contains(le_path);
							 }))
							 .repeater([](auto& ptr) {
								 if (ptr.does_match(SignatureScanner::PatternSignature::for_array_of_bytes<"48 8b 1d">())) // mov %%rbx, %0
									 return false;
								 ptr.next_instruction();
								 return true;
							 })
							 .add(3)
							 .relative_to_absolute()
							 .finalize<InterfaceReg**>();

	if (!interface_list.has_value())
		return std::nullopt;

	std::unordered_map<const char*, void*> interfaces;
	for (InterfaceReg* interface = *interface_list.value(); interface; interface = interface->m_pNext) {
		interfaces[interface->m_pName] = interface->m_CreateFn;
	}

	dlclose(handle);

	return InterfacedLibrary{ interfaces };
}

void* InterfacedLibrary::get_interface(const char* name) const
{
	for (const auto& [interfaceName, createFn] : interfaces) {
		if (std::strncmp(name, interfaceName, std::strlen(interfaceName) - 3 /*Ignore the version*/) == 0)
			return Interfaces::uncover_create_function(createFn);
	}
	return nullptr;
}

void* Interfaces::uncover_create_function(void* create_func)
{
	std::uintptr_t interface_ptr = 0;
	BCRL::pointer(Memory::mem_mgr, create_func)
		.repeater([&interface_ptr](auto& pointer) {
			if (pointer.does_match(SignatureScanner::PatternSignature::for_array_of_bytes<"48 8d 05">())) { // lea offset(%rip), %rax
				interface_ptr = pointer.clone().add(3).relative_to_absolute().get_pointer();
			} else if (pointer.does_match(SignatureScanner::PatternSignature::for_array_of_bytes<"48 8b 00">())) { // mov (%rax), %rax
				interface_ptr = *reinterpret_cast<std::uintptr_t*>(interface_ptr);
			} else if (pointer.does_match(SignatureScanner::PatternSignature::for_array_of_bytes<"c3">())) { // ret
				return false;
			}

			pointer.next_instruction();
			return true;
		});
	return reinterpret_cast<void*>(interface_ptr);
}

void Interfaces::grab_interfaces()
{
	auto client = InterfacedLibrary::create("libclient.so").value();
	auto schemasystem = InterfacedLibrary::create("libschemasystem.so").value();
	auto tier0 = InterfacedLibrary::create("libtier0.so").value();
	auto engine2 = InterfacedLibrary::create("libengine2.so").value();
	auto panorama = InterfacedLibrary::create("libpanorama.so").value();
	auto materialsystem2 = InterfacedLibrary::create("libmaterialsystem2.so").value();
	auto resourcesystem = InterfacedLibrary::create("libresourcesystem.so").value();
	auto filesystem = InterfacedLibrary::create("libfilesystem_stdio.so").value();
	auto localize = InterfacedLibrary::create("liblocalize.so").value();

	constexpr static auto INFO = [](std::string_view name, void* ptr) {
		if (ptr)
			Logging::info("Found interface '{}' at {}", name, ptr);
		else
			Logging::error("Couldn't find {}", name);
	};

	source2_client = client.get_interface<void>("Source2Client");
	INFO("Source2Client", source2_client);
	schemaSystem = schemasystem.get_interface<SchemaSystem>("SchemaSystem_");
	INFO("SchemaSystem", schemaSystem);
	engineCvar = tier0.get_interface<EngineCvar>("VEngineCvar");
	INFO("EngineCvar", engineCvar);
	engine = engine2.get_interface<EngineToClient>("Source2EngineToClient");
	INFO("Source2Engine", engine);
	panorama_ui_engine = panorama.get_interface<PanoramaUIEngine>("PanoramaUIEngine");
	INFO("PanoramaUIEngine", panorama_ui_engine);
	network_client_service = engine2.get_interface<NetworkClientService>("NetworkClientService_");
	INFO("NetworkClientService", panorama_ui_engine);
	material_system = materialsystem2.get_interface<MaterialSystem>("VMaterialSystem2_");
	INFO("VMaterialSystem2", material_system);
	resource_system = resourcesystem.get_interface<ResourceSystem>("ResourceSystem");
	INFO("ResourceSystem", resource_system);
	file_system = filesystem.get_interface<FileSystem>("VFileSystem");
	INFO("VFileSystem", file_system);
	::localize = localize.get_interface<Localize>("Localize_");
	INFO("Localize", ::localize);
}
