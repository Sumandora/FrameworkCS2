#include "SchemaClassInfo.hpp"

#include "Types/FieldData.hpp"
#include "Types/ParentData.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <span>
#include <string_view>

bool SchemaClassInfo::bases_on(SchemaClassInfo* other) const
{
	const SchemaClassInfo* curr = this;

	while (curr) {
		if (curr == other)
			return true;

		ParentData* parent = curr->parent;
		if (!parent)
			break;
		curr = parent->class_info;
	}

	return false;
}

std::int32_t SchemaClassInfo::find_offset(std::string_view field_name) const
{
	return std::ranges::find(
		std::span{ fields, fields + fieldsCount },
		field_name,
		[](const FieldData& field_data) { return field_data.fieldName; })
		->offset;
}
