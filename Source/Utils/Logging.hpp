#pragma once

#include <cerrno>
#include <concepts>
#include <cstring>
#include <format>
#include <iostream>
#include <string>
#include <string_view>
#include <type_traits>

template <typename T> requires (!std::same_as<std::remove_cvref_t<T>, void> && !std::same_as<std::remove_cvref_t<T>, char>)
struct std::formatter<T*> : public std::formatter<void*> {
	constexpr auto format(T* ptr, std::format_context& ctx) const
	{
		return std::formatter<void*>::format((void*)ptr, ctx);
	}
};

namespace Logging {
	namespace detail {
		static constexpr std::string_view RED = "\x1B[1;31m";
		static constexpr std::string_view GREEN = "\x1B[1;32m";
		static constexpr std::string_view YELLOW = "\x1B[1;33m";
		static constexpr std::string_view GREY = "\x1B[1;37m";

		static constexpr std::string_view CLEAR = "\x1B[0;39m";

		void print_raw(std::string_view color, std::string_view line, std::ostream& stream);

		template <typename... Args>
		inline void print(std::string_view color, std::format_string<Args...> fmt, Args&&... args, std::ostream& stream)
		{
			const std::string line = std::format(fmt, std::forward<Args>(args)...);
			print_raw(color, line, stream);
		}
	}

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define LOGGING_LOG_FUNCTION(name, color, stream)                                        \
	template <typename... Args>                                                          \
	inline void name(std::format_string<Args...> fmt, Args&&... args)                    \
	{                                                                                    \
		detail::print<Args...>(detail::color, fmt, std::forward<Args>(args)..., stream); \
	}

	LOGGING_LOG_FUNCTION(debug, GREY, std::clog)
	LOGGING_LOG_FUNCTION(info, GREEN, std::cout)
	LOGGING_LOG_FUNCTION(warn, YELLOW, std::cout)
	LOGGING_LOG_FUNCTION(error, RED, std::cerr)

#undef LOGGING_LOG_FUNCTION

	template <typename... Args>
	inline void perror(std::format_string<Args..., char*> fmt, Args&&... args)
	{
		error(fmt, std::forward<Args>(args)..., std::strerror(errno));
	}

}
