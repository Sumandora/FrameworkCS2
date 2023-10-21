#include "Interfaces.hpp"

#include <cstring>
#include <dlfcn.h>
#include <format>

#include "SDK/InterfaceReg.hpp"

#include "BCRL.hpp"

struct InterfacedLibrary {
	void* handle = nullptr;

	std::unordered_map<const char*, void*> interfaces{};

	explicit InterfacedLibrary(const char* path)
	{
		handle = dlmopen(LM_ID_BASE, path, RTLD_NOW | RTLD_NOLOAD | RTLD_LOCAL);
		auto interfaceList = BCRL::Session::pointer(dlsym(handle, "CreateInterface"))
						   .add(1)
						   .relativeToAbsolute()
						   .repeater([](BCRL::SafePointer& ptr) {
							   if(ptr.doesMatch("48 8b 1d")) // mov %%rbx, %0
								   return false;
							   ptr = ptr.nextInstruction();
							   return true;
						   })
						   .add(3)
						   .relativeToAbsolute()
						   .expect(std::format("Couldn't find interface list for {}", path));


		for (InterfaceReg* interface = *reinterpret_cast<InterfaceReg**>(interfaceList); interface; interface = interface->m_pNext) {
			interfaces[interface->m_pName] = interface->m_CreateFn;
		}
	}
	~InterfacedLibrary() { dlclose(handle); }

	template <typename T>
	T* getInterface(const char* name) {
		for(const auto& [interfaceName, createFn] : interfaces) {
			if(std::strncmp(name, interfaceName, std::strlen(interfaceName) - 3 /*Ignore the version*/) == 0)
				return static_cast<T*>(Interfaces::uncoverCreateFunction(createFn));
		}
		return nullptr;
	}
};

void* Interfaces::uncoverCreateFunction(void* createFunc)
{
	void* interfacePtr = nullptr;
	(void)BCRL::Session::pointer(createFunc)
		.repeater([&interfacePtr](BCRL::SafePointer& pointer) {
			if (pointer.doesMatch("48 8d 05")) { // LEA rax, [rip + offset]
				interfacePtr = pointer.add(3).relativeToAbsolute().getPointer();
			} else if (pointer.doesMatch("48 8b 00")) { // MOV rax, [rax]
				interfacePtr = *reinterpret_cast<void**>(interfacePtr);
			} else if (pointer.doesMatch("c3")) { // RET
				return false;
			}

			pointer = pointer.nextInstruction();
			return true;
		});
	return interfacePtr;
}

void Interfaces::getInterfaces()
{
	auto client = InterfacedLibrary("libclient.so");
	auto schemasystem = InterfacedLibrary("libschemasystem.so");
	auto tier0 = InterfacedLibrary("libtier0.so");

	source2Client = client.getInterface<void>("Source2Client");
	if(source2Client)
		printf("Found Source2Client interface at %p\n", source2Client);
	else
		printf("Couldn't find Source2Client\n");
	schemaSystem = schemasystem.getInterface<SchemaSystem>("SchemaSystem_");
	if(schemaSystem)
		printf("Found SchemaSystem interface at %p\n", schemaSystem);
	else
		printf("Couldn't find SchemaSystem\n");
	engineCvar = tier0.getInterface<EngineCvar>("VEngineCvar");
	if(engineCvar)
		printf("Found EngineCvar interface at %p\n", engineCvar);
	else
		printf("Couldn't find EngineCvar\n");
}
