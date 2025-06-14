#pragma once

#include "../../Padding.hpp"

struct SchemaClassInfo;

struct ParentData {
	PADDING(4);
	SchemaClassInfo* class_info; // No multi-inheritance?
};
