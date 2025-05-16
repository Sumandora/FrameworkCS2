#include "Interfaces.hpp"

#include <cstdint>
#include <cstring>
#include <dlfcn.h>
#include <format>

#include "Memory.hpp"
#include "SDK/GameClass/EngineToClient.hpp"
#include "SDK/InterfaceReg.hpp"

#include "BCRL/Session.hpp"
#include "SignatureScanner/PatternSignature.hpp"

struct InterfacedLibrary {
	std::unordered_map<const char*, void*> interfaces{};

	explicit InterfacedLibrary(const char* path)
	{
		void* handle = dlmopen(LM_ID_BASE, path, RTLD_NOW | RTLD_NOLOAD | RTLD_LOCAL);
		auto interfaceList = BCRL::pointer(Memory::mem_mgr, (std::uintptr_t)dlsym(handle, "CreateInterface"))
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

		for (InterfaceReg* interface = *interfaceList; interface; interface = interface->m_pNext) {
			interfaces[interface->m_pName] = interface->m_CreateFn;
		}

		dlclose(handle);
	}

	template <typename T>
	T* getInterface(const char* name)
	{
		for (const auto& [interfaceName, createFn] : interfaces) {
			if (std::strncmp(name, interfaceName, std::strlen(interfaceName) - 3 /*Ignore the version*/) == 0)
				return static_cast<T*>(Interfaces::uncoverCreateFunction(createFn));
		}
		return nullptr;
	}
};

void* Interfaces::uncoverCreateFunction(void* createFunc)
{
	std::uintptr_t interfacePtr = 0;
	(void)BCRL::pointer(Memory::mem_mgr, (std::uintptr_t)createFunc)
		.repeater([&interfacePtr](auto& pointer) {
			if (pointer.does_match(SignatureScanner::PatternSignature::for_array_of_bytes<"48 8d 05">())) { // LEA rax, [rip + offset]
				interfacePtr = pointer.clone().add(3).relative_to_absolute().get_pointer();
			} else if (pointer.does_match(SignatureScanner::PatternSignature::for_array_of_bytes<"48 8b 00">())) { // MOV rax, [rax]
				interfacePtr = *reinterpret_cast<std::uintptr_t*>(interfacePtr);
			} else if (pointer.does_match(SignatureScanner::PatternSignature::for_array_of_bytes<"c3">())) { // RET
				return false;
			}

			pointer.next_instruction();
			return true;
		});
	return (void*)interfacePtr;
}

void Interfaces::getInterfaces()
{
	auto client = InterfacedLibrary("libclient.so");
	auto schemasystem = InterfacedLibrary("libschemasystem.so");
	auto tier0 = InterfacedLibrary("libtier0.so");
	auto engine2 = InterfacedLibrary("libengine2.so");

	source2Client = client.getInterface<void>("Source2Client");
	if (source2Client)
		printf("Found Source2Client interface at %p\n", source2Client);
	else
		printf("Couldn't find Source2Client\n");
	schemaSystem = schemasystem.getInterface<SchemaSystem>("SchemaSystem_");
	if (schemaSystem)
		printf("Found SchemaSystem interface at %p\n", schemaSystem);
	else
		printf("Couldn't find SchemaSystem\n");
	engineCvar = tier0.getInterface<EngineCvar>("VEngineCvar");
	if (engineCvar)
		printf("Found EngineCvar interface at %p\n", engineCvar);
	else
		printf("Couldn't find EngineCvar\n");
	engine = engine2.getInterface<EngineToClient>("Source2EngineToClient");
	if (engine)
		printf("Found EngineToClient interface at %p\n", engine);
	else
		printf("Couldn't find EngineToClient\n");
}
