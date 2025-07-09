#pragma once

#include "../VirtualMethod.hpp"

#include "../CUtl/String.hpp"
#include "../CUtl/Vector.hpp"

class FileSystem {
public:
	VIRTUAL_METHOD(64, find_file_absolute_list, void,
		(UtlVector<UtlString> * vector, const char* pattern, const char* path_id),
		(this, vector, pattern, path_id));
};
