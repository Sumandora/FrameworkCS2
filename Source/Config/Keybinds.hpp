#pragma once

#include <functional>
#include <optional>
#include <utility>

#include "Key.hpp"
#include "Keybinds.hpp"

namespace Config {
	enum class KeybindKind {
		Hold,
		Toggle,
		Button,
	};

	// TODO: rewrite to use CRTP
	// Not as trivial as it seems, because of KeybindWithValue
	class Keybind {
	public:
		explicit Keybind(Key key);
		virtual ~Keybind() = default;
		virtual void processInput() = 0;

		Key key;

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(Keybind, key);
	};

	template <typename T>
	class KeybindWithValue : public Keybind {
	public:
		KeybindWithValue(T overrideValue, const Key key)
			: Keybind(key)
			, overrideValue(overrideValue)
			, enabled(false)
		{
		}
		std::optional<T> getValue() const
		{
			if (enabled) {
				return overrideValue;
			}
			return std::nullopt;
		}

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(KeybindWithValue, overrideValue);

	protected:
		T overrideValue;
		bool enabled;
	};

	template <typename T>
	class KeybindHold final : public KeybindWithValue<T> {
	public:
		KeybindHold(T overrideValue, Key key)
			: KeybindWithValue<T>(overrideValue, key)
		{
		}
		void processInput() override
		{
			this->enabled = this->key.isDown();
		}
	};

	template <typename T>
	class KeybindToggle final : public KeybindWithValue<T> {
	public:
		KeybindToggle(T overrideValue, Key key)
			: KeybindWithValue<T>(overrideValue, key)
			, keyWasDown(false)
		{
		}
		void processInput() override
		{
			if (this->key.isDown() && !keyWasDown) {
				this->enabled = !this->enabled;
			}
			keyWasDown = this->key.isDown();
		}

	private:
		bool keyWasDown;
	};

	class KeybindButton final : public Keybind {
	public:
		KeybindButton(std::function<void()> callback, const Key key)
			: Keybind(key)
			, callback(std::move(callback))
			, keyWasDown(false)
		{
		}
		void processInput() override
		{
			// TODO: code duplication but I'm too lazy to rewrite it
			if (key.isDown() && !keyWasDown) {
				callback();
			}
			keyWasDown = key.isDown();
		}

	private:
		std::function<void()> callback;
		bool keyWasDown;
	};
}