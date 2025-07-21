#include "BufferString.hpp"

#include "../../Utils/Logging.hpp"

#include "../../Libraries.hpp"

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
	static auto fptr = [] {
		auto* symbol = Libraries::tier0->get_symbol<void (*)(BufferString*, int)>("_ZN13CBufferString5PurgeEi");

		if (symbol)
			Logging::info("Found CBufferString::Purge at {}", reinterpret_cast<void*>(symbol));
		else
			Logging::error("Couldn't find CBufferString::Purge!");

		return symbol;
	}();

	fptr(this, unk1);
}

void BufferString::insert(int unk1, const char* str, int unk2, bool unk3)
{
	static auto fptr = [] {
		auto* symbol = Libraries::tier0->get_symbol<void (*)(BufferString*, int, const char*, int, bool)>("_ZN13CBufferString6InsertEiPKcib");

		if (symbol)
			Logging::info("Found CBufferString::Insert at {}", reinterpret_cast<void*>(symbol));
		else
			Logging::error("Couldn't find CBufferString::Insert!");

		return symbol;
	}();

	fptr(this, unk1, str, unk2, unk3);
}
