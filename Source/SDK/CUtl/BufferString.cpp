#include "BufferString.hpp"

#include "../../Utils/Logging.hpp"

#include <dlfcn.h>
#include <link.h>

BufferString BufferString::just(const char* str)
{
	BufferString string{
		.length = 0,
		.unk1 = 1 << 31,
		.str = nullptr,
	};

	string.insert(0, str, -1, false);

	return string;
}

BufferString::~BufferString()
{
	purge(0);
}

void BufferString::purge(int unk1)
{
	static void (*fptr)(BufferString*, int);

	if (!fptr) {
		void* handle = dlmopen(LM_ID_BASE, "libtier0.so", RTLD_NOW | RTLD_NOLOAD | RTLD_LOCAL);

		fptr = reinterpret_cast<decltype(fptr)>(dlsym(handle, "_ZN13CBufferString5PurgeEi"));

		if (fptr)
			Logging::info("Found CBufferString::Purge at {}", reinterpret_cast<void*>(fptr));
		else
			Logging::error("Couldn't find CBufferString::Purge!");

		dlclose(handle);
	}

	fptr(this, unk1);
}

void BufferString::insert(int unk1, const char* str, int unk2, bool unk3)
{
	static void (*fptr)(BufferString*, int, const char*, int, bool);

	if (!fptr) {
		void* handle = dlmopen(LM_ID_BASE, "libtier0.so", RTLD_NOW | RTLD_NOLOAD | RTLD_LOCAL);

		fptr = reinterpret_cast<decltype(fptr)>(dlsym(handle, "_ZN13CBufferString6InsertEiPKcib"));

		if (fptr)
			Logging::info("Found CBufferString::Insert at {}", reinterpret_cast<void*>(fptr));
		else
			Logging::error("Couldn't find CBufferString::Insert!");

		dlclose(handle);
	}

	fptr(this, unk1, str, unk2, unk3);
}
