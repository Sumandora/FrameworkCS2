#include "Logging.hpp"

#include <cctype>
#include <cstdio>
#include <string_view>

void Logging::detail::print_raw(std::string_view color, std::string_view line)
{
	// NOLINTNEXTLINE(modernize-use-std-print)
	std::printf("%*s[+]%*s %*s\n",
		static_cast<int>(color.size()), color.data(),
		static_cast<int>(CLEAR.size()), CLEAR.data(),
		static_cast<int>(line.size()), line.data());
}
