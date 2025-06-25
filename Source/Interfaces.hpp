#pragma once

#include <optional>
#include <unordered_map>

struct SchemaSystem;
struct EngineCvar;
struct EngineToClient;
struct PanoramaUIEngine;

class NetworkClientService;

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

	inline void* source2_client;
	inline SchemaSystem* schemaSystem;
	inline EngineCvar* engineCvar;
	inline EngineToClient* engine;
	inline PanoramaUIEngine* panorama_ui_engine;
	inline NetworkClientService* network_client_service;

	void grab_interfaces();

	void* uncover_create_function(void* create_func);
}
