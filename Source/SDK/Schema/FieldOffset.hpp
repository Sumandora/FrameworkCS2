#pragma once

#include <cstdint>
#include <type_traits>

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

// FIXME, TODO: self.classInfo() can walk up the inheritance tree through its template like properties, fix this properly soon
#define SCHEMA_VAR(type, prettyName, name)                                                             \
	inline auto&& prettyName(this auto&& self)                                                         \
	{                                                                                                  \
		static const std::int32_t offset = SchemaSystem::findOffset(self.classInfo(), name);           \
		if constexpr (std::is_const_v<std::remove_pointer_t<decltype(&self)>>)                         \
			return *reinterpret_cast<type const*>(reinterpret_cast<const std::byte*>(&self) + offset); \
		else                                                                                           \
			return *reinterpret_cast<type*>(reinterpret_cast<std::byte*>(&self) + offset);             \
	}
