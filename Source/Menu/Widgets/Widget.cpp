#include "Widget.hpp"

#include <cstdint>
#include <format>

namespace Menu {
	Widget::Widget()
		: label(std::format("##{}", reinterpret_cast<std::uintptr_t>(this)))
	{
	}

	const std::string& Widget::getLabel() const { return label; }
}