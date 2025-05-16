#pragma once

#include <cstdint>

#include "../../Interfaces.hpp"

#include "SchemaSystem.hpp"
#include "SchemaSystemTypeScope.hpp"

struct SchemaClassInfo;

#define CLASS_INFO(moduleName, className)                                                                                          \
	static inline SchemaClassInfo* classInfo()                                                                                     \
	{                                                                                                                              \
		static SchemaClassInfo* info = Interfaces::schemaSystem->FindTypeScopeForModule(moduleName)->FindDeclaredClass(className); \
		return info;                                                                                                               \
	}

#define SCHEMA_VAR(type, prettyName, name)                                            \
	inline type& prettyName()                                                    \
	{                                                                                 \
		static std::int32_t offset = SchemaSystem::findOffset(classInfo(), name);     \
		return *reinterpret_cast<type*>(reinterpret_cast<std::byte*>(this) + offset); \
	}
