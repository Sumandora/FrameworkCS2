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
		ButtonArray buttonArray{std::vector<std::string>{}, std::optional{80.0f}};
		ImVec2 lastSize{400, 300};
		Config::DPI lastDPI{Config::DPI::X1};
	};
}