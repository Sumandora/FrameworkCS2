#pragma once

#include <vector>

#include "Keybinds.hpp"

#include "libconfig.h++"

namespace Config {
	class Keybind;

	template <typename T>
	class Setting {
	public:
		explicit Setting(T value)
			: value(value)
		{
		}

		T getValue() const
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

		void setValue(T value)
		{
			this->value = value;
		}

	private:
		T value;
		std::vector<KeybindWithValue<T>*> keybinds;
	};
}