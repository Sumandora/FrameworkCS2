#pragma once

#include "Widget.hpp"

#include <string>

namespace Menu {
	class Label final : public Widget {
	public:
		explicit Label(std::string name);
		void draw() override;
	};
} // Menu