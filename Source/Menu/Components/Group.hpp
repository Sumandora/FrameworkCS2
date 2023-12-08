#pragma once

#include <memory>
#include <vector>

#include "../Widgets/Widget.hpp"

namespace Menu {
	class Group {
	public:
		explicit Group(std::string name);
		void draw() const;
		void addWidget(std::unique_ptr<Widget> widget);

	private:
		std::vector<std::unique_ptr<Widget>> widgets;
		std::string name;
	};
}