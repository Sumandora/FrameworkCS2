#include "Interfaces.hpp"

#include <cstdint>
#include <cstring>
#include <dlfcn.h>
#include <format>
#include <string_view>
#include <unordered_map>

#include "Memory.hpp"
#include "SDK/GameClass/EngineToClient.hpp"
#include "SDK/InterfaceReg.hpp"

#include "BCRL/Session.hpp"
#include "SignatureScanner/PatternSignature.hpp"
#include "Utils/Logging.hpp"

struct InterfacedLibrary {
	std::unordered_map<const char*, void*> interfaces;

	explicit InterfacedLibrary(const char* path)
	{
		void* handle = dlmopen(LM_ID_BASE, path, RTLD_NOW | RTLD_NOLOAD | RTLD_LOCAL);
		void* create_interface_fn = dlsym(handle, "CreateInterface");
		auto** interface_list = BCRL::pointer(Memory::mem_mgr, (std::uintptr_t)create_interface_fn)
									.add(1)
									.relative_to_absolute()
									.repeater([](auto& ptr) {
										if (ptr.does_match(SignatureScanner::PatternSignature::for_array_of_bytes<"48 8b 1d">())) // mov %%rbx, %0
											return false;
										ptr.next_instruction();
										return true;
									})
									.add(3)
									.relative_to_absolute()
									.expect<InterfaceReg**>(std::format("Couldn't find interface list for {}", path));

		for (InterfaceReg* interface = *interface_list; interface; interface = interface->m_pNext) {
			interfaces[interface->m_pName] = interface->m_CreateFn;
		}

		dlclose(handle);
	}

	template <typename T>
	T* get_interface(const char* name) const
	{
		for (const auto& [interfaceName, createFn] : interfaces) {
			if (std::strncmp(name, interfaceName, std::strlen(interfaceName) - 3 /*Ignore the version*/) == 0)
				return static_cast<T*>(Interfaces::uncover_create_function(createFn));
		}
		return nullptr;
	}
};

void* Interfaces::uncover_create_function(void* create_func)
{
	std::uintptr_t interface_ptr = 0;
	BCRL::pointer(Memory::mem_mgr, (std::uintptr_t)create_func)
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
	return (void*)interface_ptr;
}

void Interfaces::get_interfaces()
{
	auto client = InterfacedLibrary("libclient.so");
	auto schemasystem = InterfacedLibrary("libschemasystem.so");
	auto tier0 = InterfacedLibrary("libtier0.so");
	auto engine2 = InterfacedLibrary("libengine2.so");

	constexpr static auto INFO = [](std::string_view name, void* ptr) {
		if (ptr)
			Logging::info("Found interface '{}' at {}", name, ptr);
		else
			Logging::error("Couldn't find {}", name);
	};

	source2Client = client.get_interface<void>("Source2Client");
	INFO("Source2Client", source2Client);
	schemaSystem = schemasystem.get_interface<SchemaSystem>("SchemaSystem_");
	INFO("SchemaSystem", schemaSystem);
	engineCvar = tier0.get_interface<EngineCvar>("VEngineCvar");
	INFO("EngineCvar", engineCvar);
	engine = engine2.get_interface<EngineToClient>("Source2EngineToClient");
	INFO("Source2Engine", engine);
}
