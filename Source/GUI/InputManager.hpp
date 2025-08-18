#pragma once

#include "imgui.h"

#include "magic_enum/magic_enum.hpp"

#include <algorithm>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
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

			// Serialization keys
			static constexpr std::string_view KEY = "Key";
			static constexpr std::string_view MOUSE_BUTTON = "Mouse button";
			static constexpr std::string_view METHOD = "Method";
			static constexpr std::string_view TYPE = "Type";
			union {
				ImGuiKey key;
				ImGuiMouseButton mouse_button;
			};
			Method method : 1;
			Type type : 1;
			bool active : 1;

			template <typename BasicJsonType>
			friend void to_json(BasicJsonType& json, const Key& key)
			{
				// TODO More general, but don't know where to put: Assign string constants to these serialize/deserialize functions to not repeat the keys
				json[TYPE] = magic_enum::enum_name(key.type);

				if (key.method == Method::KEY && key.key == ImGuiKey_None)
					// empty key
					return;

				switch (key.method) {
				case Method::KEY:
					json[KEY] = key.key;
					break;
				case Method::MOUSE:
					json[MOUSE_BUTTON] = key.mouse_button;
					break;
				}
			}
			template <typename BasicJsonType>
			friend void from_json(const BasicJsonType& json, Key& key)
			{
				if (json.contains(TYPE) && json[TYPE].is_string())
					if (std::optional<Type> optional_type = magic_enum::enum_cast<Type>(json[TYPE].template get<std::string>()); optional_type.has_value())
						key.type = optional_type.value();

				if (json.contains(KEY) && json[KEY].is_number()) {
					key.key = std::clamp(json[KEY].template get<ImGuiKey>(), ImGuiKey_NamedKey_BEGIN, ImGuiKey_NamedKey_END);
					key.method = Method::KEY;
					return;
				}

				if (json.contains(MOUSE_BUTTON) && json[MOUSE_BUTTON].is_number()) {
					key.mouse_button = std::clamp(json[MOUSE_BUTTON].template get<ImGuiMouseButton>(), 0, ImGuiMouseButton_COUNT - 1);
					key.method = Method::MOUSE;
					return;
				}

				// If we survived until here then it's pointless assign an empty key
				key.method = Method::KEY;
				key.key = ImGuiKey_None;
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
