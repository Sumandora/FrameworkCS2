#pragma once

#include "../Padding.hpp"

#include "../CUtl/Vector.hpp"

#include <cstddef>

struct EconItemDefinition;

struct EconItemSchema {
private:
	PADDING(0x100);

public:
	struct ItemEntry {
		PADDING(0x8);
		EconItemDefinition* econ_item_definition;
		PADDING(0x8);
	};
	UtlVector<ItemEntry> items;
	// int count;
	// static_assert(sizeof(ItemEntry) == 0x18);
	// ItemEntry* items;

// private:
// 	PADDING(0x10);

// public:
	// int count;
};

// static_assert(offsetof(EconItemSchema, items) == 0x108);
// static_assert(offsetof(EconItemSchema, count) == 0x100);
