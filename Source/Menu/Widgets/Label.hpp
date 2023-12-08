#pragma once

#include "Widget.hpp"

#include <string>

namespace Menu {
	class Label final : public Widget {
	public:
		explicit Label(std::string text);
		void draw() override;

	private:
		const std::string text;
	};
} // Menu