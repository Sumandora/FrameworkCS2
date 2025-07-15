#pragma once

#include <cstdint>

struct UtlSymbolLarge {
	using UtlSymLargeId = std::uintptr_t;
	static constexpr UtlSymLargeId UTL_INVAL_SYMBOL_LARGE = ~0;

	const char* string() const
	{
		if (u.id == UTL_INVAL_SYMBOL_LARGE)
			return "";
		return u.string;
	}

	[[nodiscard]] bool is_valid() const
	{
		return u.id != UTL_INVAL_SYMBOL_LARGE;
	}

	union {
		UtlSymLargeId id;
		const char* string;
	} u;
};
