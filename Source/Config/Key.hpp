#pragma once

#include "nlohmann/json.hpp"
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

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(Key, code)

	private:
		explicit Key(int code);
		int code;
	};
}