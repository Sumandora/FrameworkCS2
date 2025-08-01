#pragma once

#include "Libraries.hpp"

#include <optional>
#include <unordered_map>

struct SchemaSystem;
struct EngineCvar;
struct EngineToClient;
struct PanoramaUIEngine;

class Source2Client;
class Source2ClientPrediction;
class NetworkClientService;
class MaterialSystem;
class ResourceSystem;
class FileSystem;
class Localize;

namespace Interfaces {
	class InterfacedLibrary {
		std::unordered_map<const char*, void*> interfaces;

		explicit InterfacedLibrary(std::unordered_map<const char*, void*> interfaces);

	public:
		static std::optional<InterfacedLibrary> create(const Libraries::Library& library);

		void* get_interface(const char* name) const;

		template <typename T>
		T* get_interface(const char* name) const
		{
			return static_cast<T*>(get_interface(name));
		}

		const std::unordered_map<const char*, void*>& get_interfaces() { return interfaces; }
	};

	inline Source2Client* source2_client;
	inline SchemaSystem* schemaSystem;
	inline EngineCvar* engineCvar;
	inline EngineToClient* engine;
	inline PanoramaUIEngine* panorama_ui_engine;
	inline NetworkClientService* network_client_service;
	inline MaterialSystem* material_system;
	inline ResourceSystem* resource_system;
	inline FileSystem* file_system;
	inline Localize* localize;
	inline Source2ClientPrediction* client_prediction;

	void grab_interfaces();

	void* uncover_create_function(void* create_func);
}
