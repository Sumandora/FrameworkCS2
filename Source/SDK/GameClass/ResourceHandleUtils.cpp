#include "ResourceHandleUtils.hpp"

#include "../GameClass/ResourceSystem.hpp"

#include "../../Interfaces.hpp"

#include "../../Utils/Logging.hpp"

ResourceHandleUtils* ResourceHandleUtils::get()
{
	static ResourceHandleUtils* instance
		= [] {
			  ResourceHandleUtils* resource_handle_utils = nullptr;

			  if (Interfaces::resource_system)
				  resource_handle_utils = static_cast<ResourceHandleUtils*>(Interfaces::resource_system->query_interface("ResourceHandleUtils001"));

			  if (!resource_handle_utils)
				  Logging::warn("Wasn't able to acquire ResourceHandleUtils, deleting materials will memory leak.");

			  return resource_handle_utils;
		  }();

	return instance;
}
