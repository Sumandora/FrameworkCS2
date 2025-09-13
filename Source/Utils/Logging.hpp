#pragma once

#include "glm/gtx/string_cast.hpp"

#include <cerrno>
#include <concepts>
#include <cstring>
#include <format>
#include <iostream>
#include <iterator>
#include <string>
#include <string_view>
#include <type_traits>

// TODO put custom formatters into separate file

template <typename T>
	requires(!std::same_as<std::remove_cvref_t<T>, void> && !std::same_as<std::remove_cvref_t<T>, char>)
struct std::formatter<T*> : public std::formatter<void*> {
	constexpr auto format(T* ptr, std::format_context& ctx) const
	{
		return std::formatter<void*>::format(static_cast<void*>(ptr), ctx);
	}
};

template <typename T>
	requires(!std::same_as<std::remove_cvref_t<T>, void> && !std::same_as<std::remove_cvref_t<T>, char>)
struct std::formatter<const T*> : public std::formatter<void*> {
	constexpr auto format(const T* ptr, std::format_context& ctx) const
	{
		// NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
		return std::formatter<void*>::format(const_cast<void*>(static_cast<const void*>(ptr)), ctx);
	}
};

template <typename R, typename... Args>
struct std::formatter<R (*)(Args...)> : public std::formatter<void*> {
	constexpr auto format(auto* ptr, std::format_context& ctx) const
	{
		return std::formatter<void*>::format(reinterpret_cast<void*>(ptr), ctx);
	}
};

template <typename T>
concept GLMGenType = requires(T t) {
	{ glm::detail::compute_to_string<T>::call(t) } -> std::same_as<std::string>;
};

template <typename GenType>
	requires GLMGenType<GenType>
struct std::formatter<GenType> : public std::formatter<std::string> {
	auto format(const GenType& gen_type, std::format_context& ctx) const
	{
		return std::formatter<std::string>::format(glm::to_string(gen_type), ctx);
	}
};

namespace Logging {
	namespace detail {
		static constexpr std::string_view RED = "\x1B[1;31m";
		static constexpr std::string_view GREEN = "\x1B[1;32m";
		static constexpr std::string_view YELLOW = "\x1B[1;33m";
		static constexpr std::string_view GREY = "\x1B[1;37m";

		static constexpr std::string_view CLEAR = "\x1B[0;39m";

		void print_raw(std::string_view color, std::string_view line);

		template <typename... Args>
		inline void print(std::string_view color, std::format_string<Args...> fmt, Args&&... args)
		{
			const std::string line = std::format(fmt, std::forward<Args>(args)...);
			print_raw(color, line);
		}
	}

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define LOGGING_LOG_FUNCTION(name, color)                                        \
	template <typename... Args>                                                  \
	inline void name(std::format_string<Args...> fmt, Args&&... args)            \
	{                                                                            \
		detail::print<Args...>(detail::color, fmt, std::forward<Args>(args)...); \
	}

	LOGGING_LOG_FUNCTION(debug, GREY)
	LOGGING_LOG_FUNCTION(info, GREEN)
	LOGGING_LOG_FUNCTION(warn, YELLOW)
	LOGGING_LOG_FUNCTION(error, RED)

#undef LOGGING_LOG_FUNCTION

	template <typename... Args>
	inline void perror(std::format_string<Args..., char*> fmt, Args&&... args)
	{
		error(fmt, std::forward<Args>(args)..., std::strerror(errno));
	}

}
