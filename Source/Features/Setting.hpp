#pragma once

#include "imgui.h"
#include "nlohmann/json.hpp"

#include <cstring>
#include <functional>
#include <string.h>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include "magic_enum/magic_enum.hpp"

class Setting;

class SettingsHolder {
protected:
	// NOLINTNEXTLINE(cppcoreguidelines-non-private-member-variables-in-classes)
	std::vector<Setting*> settings;

	friend Setting;

	void render_all_childs();

public:
	// This saves the &
	// NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
	operator SettingsHolder*() { return this; }
};

class Setting {
	std::string name;
	std::function<bool()> visible;

public:
	explicit Setting(SettingsHolder* parent, std::string name);
	virtual ~Setting() = default;

	const std::string& get_name() { return name; }

	virtual void render() = 0;
	virtual void serialize(nlohmann::json& output_json) const = 0;
	virtual void deserialize(const nlohmann::json& input_json) = 0;

	friend void to_json(nlohmann::json& json, const Setting& setting) { setting.serialize(json); }
	friend void from_json(const nlohmann::json& json, Setting& setting) { setting.deserialize(json); }

	bool is_visible();
	void add_visible_condition(std::function<bool()> visible) &;
};

class Checkbox : public Setting {
	bool value;

public:
	Checkbox(SettingsHolder* parent, std::string name, bool value);

	[[nodiscard]] bool get() const { return value; }

	void render() override;
	void serialize(nlohmann::json& output_json) const override;
	void deserialize(const nlohmann::json& input_json) override;
};

class FloatSlider : public Setting {
	float value = 0.0F, min, max;

public:
	FloatSlider(SettingsHolder* parent, std::string name, float min, float max);

	[[nodiscard]] float get() const { return value; }

	void render() override;
	void serialize(nlohmann::json& output_json) const override;
	void deserialize(const nlohmann::json& input_json) override;
};

class Color : public Setting {
	ImColor value;

public:
	Color(SettingsHolder* parent, std::string name, ImColor value = { 1.0F, 1.0F, 1.0F, 1.0F });

	[[nodiscard]] ImColor get() const { return value; }

	void render() override;
	void serialize(nlohmann::json& output_json) const override;
	void deserialize(const nlohmann::json& input_json) override;
};

template <typename E>
	requires std::is_enum_v<E>
class Combo : public Setting {
	std::unordered_map<E, std::string> names;
	E value;

	static std::unordered_map<E, std::string> generate_names()
	{
		std::unordered_map<E, std::string> names;
		// TODO case conversion
		for (auto [value, name] : magic_enum::enum_entries<E>()) {
			names[value] = name;
		}
		return names;
	}

public:
	Combo(SettingsHolder* parent, std::string name, E value = magic_enum::enum_values<E>()[0], std::unordered_map<E, std::string> names = generate_names())
		: Setting(parent, std::move(name))
		, value(value)
		, names(std::move(names))
	{
	}

	[[nodiscard]] E get() const { return value; }

	void render() override
	{
		// TODO this iteration is slow, but I can't iterate over the map, because its unordered and even std::map does not keep order (ordered_map soonTM)
		if (ImGui::BeginCombo(get_name().c_str(), names[value].c_str(), ImGuiComboFlags_None)) {
			for (E value : magic_enum::enum_values<E>()) {
				bool selected = value == this->value;
				if (ImGui::Selectable(names[value].c_str(), selected))
					this->value = value;

				if (selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
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

class Button : public Setting {
	std::function<void()> action;

public:
	Button(SettingsHolder* parent, std::string name, std::function<void()> action);

	void render() override;
	void serialize(nlohmann::json& output_json) const override;
	void deserialize(const nlohmann::json& input_json) override;
};

class Subgroup : public Setting, public SettingsHolder {
	bool anonymous;

public:
	Subgroup(SettingsHolder* parent, std::string name, bool anonymous = false);

	void render() override;
	void serialize(nlohmann::json& output_json) const override;
	void deserialize(const nlohmann::json& input_json) override;
};

class MetaSetting : public Setting, public SettingsHolder {
public:
	MetaSetting(SettingsHolder* parent, std::string name);

	void render() override;
	void serialize(nlohmann::json& output_json) const override;
	void deserialize(const nlohmann::json& input_json) override;
};

class Tabs : public Setting, public SettingsHolder {
public:
	explicit Tabs(SettingsHolder* parent, std::string name);

	void render() override;
	void serialize(nlohmann::json& output_json) const override;
	void deserialize(const nlohmann::json& input_json) override;
};
