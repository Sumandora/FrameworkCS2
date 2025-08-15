#pragma once

#include "../Padding.hpp"

#include "../CUtl/Vector.hpp"

#include <cstddef>

struct EconItemDefinition;

struct EconItemSchema {
private:
	PADDING(0xf8);

public:
	struct ItemEntry {
		int id;
		PADDING(0x4);
		EconItemDefinition* econ_item_definition;
		int unk;
		PADDING(0x4);
	};
	UtlVector<ItemEntry> items;
};
