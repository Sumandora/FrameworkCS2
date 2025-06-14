#pragma once

#include "../Padding.hpp"

#include <cstdint>

struct FieldData;
struct StaticFieldData;
struct BaseClassData;
struct ParentData;

struct SchemaClassInfo {
	void* self; // This struct points to itself, no clue why

	char* className;
	char* moduleName;

	std::uint32_t size;

	std::uint16_t fieldsCount;
	std::uint16_t staticFieldsCount;

private:
	PADDING(8);
public:

	FieldData* fields;
	StaticFieldData* staticFields;
	ParentData* parent;

	// Checks if this class bases on the `other` class info (even if it is several base classes deep)
	bool bases_on(SchemaClassInfo* other) const;
};
