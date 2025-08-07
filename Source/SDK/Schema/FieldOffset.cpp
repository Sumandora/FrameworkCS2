#include "FieldOffset.hpp"

#include "SchemaClassInfo.hpp"
#include "SchemaSystem.hpp"
#include "Types/FieldData.hpp"

#include "../../Utils/Logging.hpp"

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string_view>

std::int32_t SchemaSystem::findOffset(SchemaClassInfo* class_info, const char* field_name)
{
	for (std::size_t i = 0; i < class_info->fieldsCount; i++) {
		const FieldData& field = class_info->fields[i];
		if (std::string_view{ field.fieldName } == field_name) {
			return field.offset;
		}
	}
	// TODO This is too suttle, I don't see it most of the time
	Logging::error("Failed to find offset for {}", field_name);
	return 0;
}
