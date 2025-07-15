#pragma once

#include "../Padding.hpp"

struct EconItemSchema;

struct EconItemSystem {
private:
	PADDING(0x8);

public:
	EconItemSchema* item_schema;
};
