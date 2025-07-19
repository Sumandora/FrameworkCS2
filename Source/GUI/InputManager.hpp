#pragma once

#include "imgui.h"

#include "nlohmann/json.hpp"

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

			template <typename BasicJsonType>
			friend void to_json(BasicJsonType& json, const Key& key)
			{
				json["Name"] = key.name;
				json["Method"] = key.method;
				json["Type"] = key.type;
				json["Active"] = key.active;
				switch(key.method) {
				case InputManager::Key::Method::KEY:
					json["Key"] = key.key;
					break;
				case InputManager::Key::Method::MOUSE:
					json["Mouse Button"] = key.mouse_button;
					break;
				}
			}
			template <typename BasicJsonType>
			friend void from_json(const BasicJsonType& json, Key& key)
			{
				json.at("Name").get_to(key.name);
				key.method = json["Method"];
				key.type = json["Type"];
				key.active = json["Active"];

				switch(key.method) {
				case InputManager::Key::Method::KEY:
					json.at("Key").get_to(key.key);
					break;
				case InputManager::Key::Method::MOUSE:
					json.at("Mouse Button").get_to(key.mouse_button);
					break;
				}
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
