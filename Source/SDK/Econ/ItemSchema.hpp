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
};
