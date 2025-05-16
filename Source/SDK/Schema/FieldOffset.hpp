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

#define SCHEMA_VAR(type, prettyName, name)                                               \
	inline type* prettyName##Ptr()                                                       \
	{                                                                                    \
		static std::int32_t offset = findOffset(classInfo(), name);                      \
		return reinterpret_cast<type*>(reinterpret_cast<std::uintptr_t>(this) + offset); \
	}                                                                                    \
	inline type& prettyName()                                                            \
	{                                                                                    \
		return *prettyName##Ptr();                                                       \
	}
	
