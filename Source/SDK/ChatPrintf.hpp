#pragma once

#include "imgui.h"

#include <format>
#include <string>

namespace ChatPrintf {
	void resolve_signatures();
	void print(const std::string& text);
	std::string to_html_rgba(const ImColor& color);

	template <typename... Args>
	inline void print(std::format_string<Args...> fmt, Args&&... args)
	{
		print(std::format(fmt, std::forward<Args>(args)...));
	}
}
