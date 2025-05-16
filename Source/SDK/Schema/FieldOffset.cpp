#include "FieldOffset.hpp"
#include <cstdio>
#include <cstring>

#include "SchemaClassInfo.hpp"
#include "SchemaSystem.hpp"
#include "Types/FieldData.hpp"

std::int32_t SchemaSystem::findOffset(SchemaClassInfo* classInfo, const char* fieldName)
{
	for (std::size_t i = 0; i < classInfo->fieldsCount; i++) {
		FieldData& field = classInfo->fields[i];
		if (strcmp(field.fieldName, fieldName) == 0) {
			return field.offset;
		}
	}
	printf("Failed to find offset for %s\n", fieldName);
	return 0;
}
