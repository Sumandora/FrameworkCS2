#pragma once

#include "../../Interfaces.hpp" // IWYU pragma: keep

#include "SchemaClassInfo.hpp"
#include "SchemaSystem.hpp"
#include "SchemaSystemTypeScope.hpp"

#include <type_traits>

#define CLASS_INFO(moduleName, className)                                                                                            \
	static inline SchemaClassInfo* classInfo()                                                                                       \
	{                                                                                                                                \
		static SchemaClassInfo* info = Interfaces::schemaSystem->FindTypeScopeForModule(moduleName)->find_declared_class(className); \
		return info;                                                                                                                 \
	}

#define SCHEMA_VAR(type, prettyName, name)                                                     \
	using prettyName##_type = type;                                                            \
	inline auto& prettyName()                                                                  \
	{                                                                                          \
		static const std::int32_t offset = classInfo()->find_offset(name);                     \
		using T = type;                                                                        \
		return *reinterpret_cast<T*>(reinterpret_cast<std::byte*>(this) + offset);             \
	}                                                                                          \
	inline auto& prettyName() const                                                            \
	{                                                                                          \
		static const std::int32_t offset = classInfo()->find_offset(name);                     \
		using T = type;                                                                        \
		return *reinterpret_cast<T const*>(reinterpret_cast<const std::byte*>(this) + offset); \
	}

// Basically dynamic_cast for schema types.
template <typename T, typename R>
T schema_cast(R from, SchemaClassInfo* schema_class_info = std::remove_pointer_t<R>::classInfo())
{
	if (schema_class_info->bases_on(std::remove_pointer_t<T>::classInfo()))
		return static_cast<T>(from);
	return nullptr;
}
