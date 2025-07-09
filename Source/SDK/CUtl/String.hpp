#pragma once

struct UtlString {
	char* string;

	~UtlString();

	// NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
	constexpr operator char*() const { return string; }
};
