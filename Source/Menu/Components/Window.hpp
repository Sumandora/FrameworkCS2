#pragma once

#include <string>
#include <vector>

#include "../Widgets/ButtonArray.hpp"
#include "Tab.hpp"
#include "../../Config/Config.hpp"

#include <imgui.h>
#include <memory>

namespace Menu {
	class Window {
	public:
		explicit Window(std::string name);
		void addTab(std::unique_ptr<Tab> tab);
		void draw();

	private:
		std::string name;
		std::vector<std::unique_ptr<Tab>> tabs;
		ImVec2 lastSize{400, 300};
		ButtonArray tabSelector{std::vector<std::string>{}};
		Config::DPI lastDPI{Config::DPI::X1};
	};
}