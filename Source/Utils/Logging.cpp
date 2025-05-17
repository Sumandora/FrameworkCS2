#include "Logging.hpp"

#include <cctype>
#include <iostream>
#include <string_view>

void Logging::detail::print_raw(std::string_view color, std::string_view line, std::ostream& stream)
{
	// NOLINTNEXTLINE(performance-avoid-endl)
	stream << color << "[+] " << CLEAR << line << std::endl;
}
