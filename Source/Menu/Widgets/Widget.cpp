#include "Widget.hpp"

#include <cstdint>
#include <format>

namespace Menu {
	std::string Widget::getLabel()
	{
		return std::format("##{}", reinterpret_cast<std::uintptr_t>(this));
	}
}