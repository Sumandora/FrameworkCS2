#pragma once

#include <vector>

#include "Keybinds.hpp"

namespace Config {

	class Keybind;

	template <typename T>
	class Setting {
	public:
		explicit Setting(T&& value)
			: value(std::move(value))
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

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(Setting<T>, value);

	private:
		T value;
		std::vector<KeybindWithValue<T>*> keybinds;
	};
}