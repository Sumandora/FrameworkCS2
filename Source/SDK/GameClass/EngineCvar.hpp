#pragma once

#include "../VirtualMethod.hpp"
#include "../Padding.hpp"

#include "../CUtl/LinkedList.hpp"

struct ConVar;

struct EngineCvar {
	using CvarIterator = void*;
	VIRTUAL_METHOD(12, getFirstCvarIterator, CvarIterator, (), (this));
	VIRTUAL_METHOD(13, getNextCvarIterator, CvarIterator, (CvarIterator it), (this, it));
	VIRTUAL_METHOD(37, getCvar, ConVar*, (CvarIterator it), (this, it));

	PADDING(0x40);
	CUtlLinkedList<ConVar*> convarList;

	// TODO by name
};