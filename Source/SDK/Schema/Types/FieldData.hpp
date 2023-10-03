#pragma once

#include "../../Padding.hpp"

#include <cstdint>

struct SchemaType;

struct FieldData {
	char* fieldName;
	SchemaType* type;
	std::int32_t offset;
	PADDING(12);
};