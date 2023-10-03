#pragma once

#include "../../Padding.hpp"

struct Flag;

struct SchemaType {
	PADDING(8);

	char* name;

	PADDING(8);

	Flag* arrayOfFlags;
};