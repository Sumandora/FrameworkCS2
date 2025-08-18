#include "SchemaClassInfo.hpp"

#include "Types/FieldData.hpp"
#include "Types/ParentData.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <format>
#include <span>
#include <stdexcept>
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
	const std::span fields_span{ fields, fields + fieldsCount };
	const auto it = std::ranges::find(
		fields_span,
		field_name,
		[](const FieldData& field_data) { return field_data.fieldName; });
	if (it == fields_span.end())
		throw std::runtime_error{ std::format("Couldn't find schema variable {}", field_name) };
	return it->offset;
}
