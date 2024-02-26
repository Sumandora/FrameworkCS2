#pragma once

#include "../../Config/Setting.hpp"
#include "Widget.hpp"

namespace Menu {
	class TextInput final : public Widget {
	public:
		TextInput(std::string name, Config::Setting<std::string>& setting);
		TextInput(std::string name, std::string& string);
		void draw() override;

	private:
		std::string& ownedString;
	};
} // Menu
