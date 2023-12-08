#pragma once

#include <string_view>

enum ImGuiKey : int;

namespace Config {
	class Key {
	public:
		static Key disabled();
		static Key fromKey(ImGuiKey key);
		static Key fromMouseButton(int mouseButton);
		[[nodiscard]] bool isDown() const;
		[[nodiscard]] std::string_view toString() const;

	private:
		explicit Key(int code);
		int code;
	};
}