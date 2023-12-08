#pragma once
#include <format>
#include <iostream>

namespace Log {
	enum class Level {
		Debug,
		Info,
		Warning,
		Error,
		Critical,
	};

	// TODO: let the users specify the level
	// Maybe during compilation, maybe in menu
	inline Level logLevel = Level::Info;

	namespace {
		std::string_view getPrefix(Level level)
		{
			static constexpr std::string_view prefixes[] = {
				"[debug] ",
				"[info] ",
				"[warning] ",
				"[error] ",
				"[critical] ",
			};
			return prefixes[static_cast<int>(level)];
		}

		std::string_view getANSIColor(Level level)
		{
			static constexpr std::string_view colors[] = {
				"\033[0m",
				"\033[0;32m",
				"\033[0;33m",
				"\033[0;31m",
				"\033[0;31m",
			};
			return colors[static_cast<int>(level)];
		}
	}

	template <typename... Args>
	void log(const Level level, std::format_string<Args...> fmt, Args&&... args)
	{
		if (level < logLevel) {
			return;
		}
		const std::string_view color = getANSIColor(level);
		const std::string_view prefix = getPrefix(level);
		const std::string_view message = std::format(fmt, std::forward<Args>(args)...);
		static constexpr std::string_view clear_color = "\033[0m";
		std::cout << color << prefix << ' ' << message << clear_color << std::endl;
	}
}