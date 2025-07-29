#pragma once

#include "../VirtualMethod.hpp"
#include "../Padding.hpp"

#include "../CUtl/LinkedList.hpp"

#include <cstdint>

struct ConVar;

struct EngineCvar {
	using CvarIterator = void*;
	VIRTUAL_METHOD(12, getFirstCvarIterator, CvarIterator, (), (this));
	VIRTUAL_METHOD(13, getNextCvarIterator, CvarIterator, (CvarIterator it), (this, it));
	VIRTUAL_METHOD(41, getCvar, ConVar*, (CvarIterator it), (this, it));

	// TODO This works somewhat, but is still wrong I think, sometimes there's nullptr in between normal convars...
	PADDING(0x42);
	std::int16_t convar_count;
	PADDING(0x4);
	CUtlLinkedList<ConVar*> convarList;

	ConVar* findByName(const char* name);
};
