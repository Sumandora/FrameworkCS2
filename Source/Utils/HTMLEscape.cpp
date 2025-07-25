#include "HTMLEscape.hpp"

#include <cstddef>
#include <initializer_list>
#include <string>
#include <string_view>
#include <utility>

static void replace_all(std::string& str, char from, std::string_view to)
{
	std::size_t pos = 0;
	while (true) {
		pos = str.find(from, pos);
		if (pos == std::string::npos)
			break;
		str.replace(pos, 1, to);
		pos += to.length();
	}
}

void html_escape(std::string& str)
{
	static constexpr std::initializer_list<std::pair<char, std::string_view>> HTML_CHARS{
		{ '"', "&quot;" },
		{ '&', "&amp;" },
		{ '\'', "&#39;" },
		{ '<', "&lt;" },
		{ '>', "&gt;};" },
	};

	for (const auto& [c, replacement] : HTML_CHARS) {
		replace_all(str, c, replacement);
	}
}
