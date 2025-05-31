#pragma once

#include <concepts>
#include <optional>
#include <unordered_map>

struct SchemaSystem;
struct EngineCvar;
struct EngineToClient;

namespace Interfaces {
	class InterfacedLibrary {
		std::unordered_map<const char*, void*> interfaces;

		explicit InterfacedLibrary(std::unordered_map<const char*, void*> interfaces);
	public:
		static std::optional<InterfacedLibrary> create(const char* path);

		void* get_interface(const char* name) const;

		template <typename T>
		T* get_interface(const char* name) const
		{
			return static_cast<T*>(get_interface(name));
		}

		const std::unordered_map<const char*, void*>& get_interfaces() { return interfaces; }
	};

	inline void* source2Client;
	inline SchemaSystem* schemaSystem;
	inline EngineCvar* engineCvar;
	inline EngineToClient* engine;

	void grab_interfaces();

	void* uncover_create_function(void* create_func);
}
