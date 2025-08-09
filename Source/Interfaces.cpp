#include "Interfaces.hpp"

#include "Libraries.hpp"
#include "SDK/GameClass/GameTypes.hpp"
#include "SDK/GameClass/Source2Client.hpp"
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

InterfacedLibrary::InterfacedLibrary(std::unordered_map<std::string_view, void*> interfaces)
	: interfaces(std::move(interfaces))
{
}

std::optional<InterfacedLibrary> InterfacedLibrary::create(const Libraries::Library& library)
{
	void* create_interface_fn = library.get_symbol<void*>("CreateInterface");
	if (!create_interface_fn)
		return std::nullopt;

	std::expected<InterfaceReg**, BCRL::FinalizationError> interface_list
		= BCRL::pointer(Memory::mem_mgr, create_interface_fn)
			  .filter(
				  BCRL::everything(Memory::mem_mgr)
					  .with_flags("r-x")
					  .also([library_name = library.get_library_name()](const decltype(Memory::mem_mgr)::RegionT& r) {
						  auto path = r.get_path();
						  if (!path.has_value())
							  return false;
						  return path->contains(library_name);
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

	std::unordered_map<std::string_view, void*> interfaces;
	for (InterfaceReg* interface = *interface_list.value(); interface; interface = interface->m_pNext) {
		interfaces[interface->m_pName] = interface->m_CreateFn;
	}

	return InterfacedLibrary{ interfaces };
}

void* InterfacedLibrary::get_interface(std::string_view name) const
{
	for (const auto& [interfaceName, createFn] : interfaces) {
		if (interfaceName.substr(0, interfaceName.length() - 3 /*Ignore the version*/) == name)
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
	auto client = InterfacedLibrary::create(Libraries::client).value();
	auto schemasystem = InterfacedLibrary::create(Libraries::schemasystem).value();
	auto tier0 = InterfacedLibrary::create(Libraries::tier0).value();
	auto engine2 = InterfacedLibrary::create(Libraries::engine2).value();
	auto panorama = InterfacedLibrary::create(Libraries::panorama).value();
	auto materialsystem2 = InterfacedLibrary::create(Libraries::materialsystem2).value();
	auto resourcesystem = InterfacedLibrary::create(Libraries::resourcesystem).value();
	auto filesystem = InterfacedLibrary::create(Libraries::filesystem).value();
	auto localize = InterfacedLibrary::create(Libraries::localize).value();
	auto particles = InterfacedLibrary::create(Libraries::particles).value();
	auto matchmaking = InterfacedLibrary::create(Libraries::matchmaking).value();

	constexpr static auto INFO = [](std::string_view name, void* ptr) {
		if (ptr)
			Logging::info("Found interface '{}' at {}", name, ptr);
		else
			Logging::error("Couldn't find {}", name);
	};

	source2_client = client.get_interface<Source2Client>("Source2Client");
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
	client_prediction = client.get_interface<Source2ClientPrediction>("Source2ClientPrediction");
	INFO("Source2ClientPrediction", client_prediction);
	particle_system_mgr = particles.get_interface<ParticleSystemMgr>("ParticleSystemMgr");
	INFO("ParticleSystemMgr", particle_system_mgr);
	game_types = matchmaking.get_interface<GameTypes>("GameTypes");
	INFO("GameTypes", game_types);
}
