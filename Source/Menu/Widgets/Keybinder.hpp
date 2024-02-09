#pragma once

#include "Widget.hpp"

#include <string>

namespace Config {
	class Key;
}

namespace Menu {
	class Keybinder final : public Widget {
	public:
		Keybinder(std::string name, Config::Key& key);
		static void draw(const std::string& name, Config::Key& key);
		void draw() override;

	private:
		Config::Key& key;
	};
}