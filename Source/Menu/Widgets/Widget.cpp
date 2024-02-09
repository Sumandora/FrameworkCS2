#include "Widget.hpp"

#include <cstdint>
#include <format>
#include <utility>

namespace Menu {
	Widget::Widget(std::string name)
		: name(std::move(name))
		, label(std::format("##{}", reinterpret_cast<std::uintptr_t>(this)))
	{
	}

	const std::string& Widget::getLabel() const { return label; }
}