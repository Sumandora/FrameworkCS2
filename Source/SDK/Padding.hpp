#pragma once

#include <cstddef>

#define CONCAT(a, b) a##b
#define PADDING_NAME(line) CONCAT(PAD, line)

#define PADDING(size) \
	[[maybe_unused]] std::byte PADDING_NAME(__LINE__)[size] { }

#define OFFSET(type, name, offset)                                                             \
	constexpr auto& name()                                                                     \
	{                                                                                          \
		using T = type;                                                                        \
		return *reinterpret_cast<T*>(reinterpret_cast<std::byte*>(this) + offset);             \
	}                                                                                          \
	constexpr auto& name() const                                                               \
	{                                                                                          \
		using T = type;                                                                        \
		return *reinterpret_cast<T const*>(reinterpret_cast<const std::byte*>(this) + offset); \
	}
