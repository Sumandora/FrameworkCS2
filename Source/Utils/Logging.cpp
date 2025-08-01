#include "Logging.hpp"

#include <cctype>
#include <iostream>
#include <print>
#include <string_view>

void Logging::detail::print_raw(std::string_view color, std::string_view line, std::ostream& stream)
{
	std::println(stream, "{}[+]{} {}", color, CLEAR, line);
}
