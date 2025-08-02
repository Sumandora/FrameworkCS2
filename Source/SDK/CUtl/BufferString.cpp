#include "BufferString.hpp"

#include "../../Libraries.hpp"

#include "../../Memory.hpp"

#include "RetAddrSpoofer.hpp"

#include <dlfcn.h>
#include <link.h>

static void (*purge)(BufferString*, int) = nullptr;
static void (*insert)(BufferString*, int, const char*, int, bool) = nullptr;

void BufferString::resolve_functions()
{
	::purge = Libraries::tier0->get_symbol<decltype(::purge)>("_ZN13CBufferString5PurgeEi");
	Memory::accept("CBufferString::Purge", ::purge);
	::insert = Libraries::tier0->get_symbol<decltype(::insert)>("_ZN13CBufferString6InsertEiPKcib");
	Memory::accept("CBufferString::Insert", ::insert);
}

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
	RetAddrSpoofer::invoke(::purge, this, unk1);
}

void BufferString::insert(int unk1, const char* str, int unk2, bool unk3)
{
	RetAddrSpoofer::invoke(::insert, this, unk1, str, unk2, unk3);
}
