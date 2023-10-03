#pragma once

#include "../Padding.hpp"

#include <cstdint>

struct FieldData;
struct StaticFieldData;
struct BaseClassData;

struct SchemaClassInfo {
	void* self; // This struct points to itself, no clue why

	char* className;
	char* moduleName;

	std::uint32_t size;

	std::uint8_t fieldsCount;

	PADDING(4); // no clue what this is

	std::uint8_t baseClassCount;

	PADDING(4);

	FieldData* fields;
	StaticFieldData* staticFields;

	BaseClassData* baseClass;
};
