#pragma once

#include "imgui.h"

#include <cstdint>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

namespace GUI {
	class InputManager {
	public:
		class Key {
			enum class Method : std::uint8_t {
				KEY,
				MOUSE,
			};
			enum class Type : std::uint8_t {
				HOLD,
				TOGGLE,
			};

			std::string name;
			Method method : 1;
			Type type : 1;
			bool active : 1;
			// Which ever type may be bigger is taken here.
			union {
				ImGuiKey key;
				ImGuiMouseButton mouse_button;
			};

		public:
			[[nodiscard]] bool is_active() const;

			void update_state();
			void draw();
		};

	private:
		std::vector<std::weak_ptr<Key>> keys;

	public:
		void register_key(std::weak_ptr<Key> key);

		void update_states();
	};

	InputManager& get_input_manager();
}
