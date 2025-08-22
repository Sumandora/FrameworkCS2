#pragma once

#include "../BindableSetting.hpp"
#include "../Setting.hpp"

#include "magic_enum/magic_enum.hpp"

#include "../../GUI/Elements/EnumCombo.hpp"

#include <string>
#include <utility>

template <typename E>
	requires std::is_enum_v<E>
class RawCombo : public Setting {
	E value;
	// TODO thread-safety?

public:
	RawCombo(SettingsHolder* parent, std::string name, E value = magic_enum::enum_values<E>()[0])
		: Setting(parent, std::move(name))
		, value(value)
	{
	}

	[[nodiscard]] E get() const { return value; }
	void set(E new_value) { value = new_value; }

	void render() override
	{
		ImGuiExt::EnumCombo(get_name().c_str(), value);
	}
	void serialize(nlohmann::json& output_json) const override
	{
		output_json = std::to_underlying(value);
	}
	void deserialize(const nlohmann::json& input_json) override
	{
		value = static_cast<E>(input_json);
	}
};

template <typename E>
using Combo = BindableSetting<RawCombo<E>>;
