#pragma once

#include "../../Padding.hpp"

struct SchemaType;

struct StaticFieldData {
	char* fieldName;
	SchemaType* type;
	void* data; // content of field
	PADDING(16);
};