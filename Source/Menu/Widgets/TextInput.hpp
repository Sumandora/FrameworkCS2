#pragma once

#include "../../Config/Setting.hpp"
#include "Widget.hpp"

namespace Menu {
	class TextInput final : public Widget {
	public:
		TextInput(std::string name, Config::Setting<std::string>& setting);
		void draw() override;

	private:
		Config::Setting<std::string>& setting;
	};
} // Menu
