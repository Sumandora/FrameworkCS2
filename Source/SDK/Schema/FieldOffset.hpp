#pragma once

#include <cstdint>

#include "../../Interfaces.hpp"
#include "SchemaSystem.hpp"
#include "SchemaSystemTypeScope.hpp"

struct SchemaClassInfo;

std::int32_t findOffset(SchemaClassInfo* classInfo, const char* fieldName);

#define CLASS_INFO(moduleName, className)                                                                                          \
	static inline SchemaClassInfo* classInfo()                                                                                     \
	{                                                                                                                              \
		static SchemaClassInfo* info = Interfaces::schemaSystem->FindTypeScopeForModule(moduleName)->FindDeclaredClass(className); \
		return info;                                                                                                               \
	}

// Defines a field of type `type` with name `prettyName` at offset `offset` (measured in bytes) from the field named `varPrettyName`.
// That variable needs to be defined first.
#define SCHEMA_VAR_OFFSET(type, prettyName, varPrettyName, offset)                                      \
	inline type* prettyName##Ptr() const                                                                \
	{                                                                                                   \
		return reinterpret_cast<type*>(reinterpret_cast<std::uint8_t*>(varPrettyName##Ptr()) + offset); \
	}                                                                                                   \
	inline type prettyName() const { return *prettyName##Ptr(); }

#define SCHEMA_VAR(type, prettyName, name)                                               \
	inline type* prettyName##Ptr() const                                                 \
	{                                                                                    \
		static std::int32_t offset = findOffset(classInfo(), name);                      \
		return reinterpret_cast<type*>(reinterpret_cast<std::uintptr_t>(this) + offset); \
	}                                                                                    \
	inline type& prettyName() const                                                      \
	{                                                                                    \
		return *prettyName##Ptr();                                                       \
	}