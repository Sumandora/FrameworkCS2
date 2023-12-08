#pragma once

#include "Widget.hpp"

#include <string>

namespace Config {
	class Key;
}

namespace Menu {
	class Keybinder : public Widget {
	public:
		Keybinder(std::string label, Config::Key* key);
		static void draw(const std::string& label, Config::Key& key);
		void draw() override;

	private:
		const std::string label;
		Config::Key* key;
	};
}