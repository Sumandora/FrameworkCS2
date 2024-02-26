#pragma once

#include <vector>

#include "Keybinds.hpp"

#include "libconfig.h++"

namespace Config {
	template<typename T>
	using SerializableType = std::conditional_t<std::is_enum_v<T>, std::underlying_type_t<T>, T>;

	class Keybind;

	template <typename T>
	class Setting {
	public:
		explicit Setting(T value)
			: value(value)
		{
		}

		const T& getValue() const
		{
			for (const auto& keybind : keybinds) {
				if (auto overrideValue = keybind->getValue()) {
					return *overrideValue;
				}
			}
			return value;
		}

		T& getValue()
		{
			for (const auto& keybind : keybinds) {
				if (auto overrideValue = keybind->getValue()) {
					return *overrideValue;
				}
			}
			return value;
		}

		T* getPointer()
		{
			return &value;
		}

		T operator*() const
		{
			return getValue();
		}

		void setValue(T&& value)
		{
			this->value = std::move(value);
		}

		void serialize(const std::string& name /* TODO Member variable */, libconfig::Setting& settingsGroup) {
			settingsGroup[name] = static_cast<SerializableType<T>>(value);
		}

		void deserialize(const std::string& name /* TODO Member variable */, libconfig::Setting& settingsGroup) {
			settingsGroup[name] = static_cast<SerializableType<T>>(value);
		}

	private:
		T value;
		std::vector<KeybindWithValue<T>*> keybinds;
	};
}