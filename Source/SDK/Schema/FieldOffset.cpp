#include <cstdio>
#include <cstring>

#include "SchemaClassInfo.hpp"
#include "SchemaSystem.hpp"
#include "Types/FieldData.hpp"

#include "../../Utils/Logging.hpp"

std::int32_t SchemaSystem::findOffset(SchemaClassInfo* classInfo, const char* fieldName)
{
	for (std::size_t i = 0; i < classInfo->fieldsCount; i++) {
		FieldData& field = classInfo->fields[i];
		if (strcmp(field.fieldName, fieldName) == 0) {
			return field.offset;
		}
	}
	// TODO This is too suttle, I don't see it most of the time
	Logging::error("Failed to find offset for {}", fieldName);
	return 0;
}
