#pragma once

#include "../VirtualMethod.hpp"

class ResourceHandleUtils {
public:
	VIRTUAL_METHOD(2, delete_resource, void, (void* resource), (this, resource));
};
