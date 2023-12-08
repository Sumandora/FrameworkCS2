#pragma once
#include <string_view>

namespace Menu {
	class Widget {
	public:
		virtual ~Widget() = default;
		virtual void draw() = 0;
		std::string getLabel();
	};
}