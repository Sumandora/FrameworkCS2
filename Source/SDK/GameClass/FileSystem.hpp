#pragma once

#include "../VirtualMethod.hpp"

#include "../CUtl/Buffer.hpp"
#include "../CUtl/String.hpp"
#include "../CUtl/Vector.hpp"

using FSAllocFunc = void* (*)(const char* filename, unsigned int bytes);

class FileSystem {
public:
	VIRTUAL_METHOD(25, read_file, bool,
		(const char* file_name, const char* path, UtlBuffer* buf, int max_bytes, long starting_byte, FSAllocFunc alloc_func = nullptr),
		(this, file_name, path, buf, max_bytes, starting_byte, alloc_func));
	VIRTUAL_METHOD(64, find_file_absolute_list, void,
		(UtlVector<UtlString> * vector, const char* pattern, const char* path_id),
		(this, vector, pattern, path_id));
};
