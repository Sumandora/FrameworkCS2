#pragma once

#include "imgui.h"
#include "nlohmann/json.hpp"
#include "nlohmann/json_fwd.hpp"

#include <functional>
#include <string>
#include <utility>
#include <vector>

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
	std::function<void()> visible;

public:
	explicit Setting(SettingsHolder* parent, std::string name);
	virtual ~Setting() = default;

	const std::string& get_name() { return name; }

	virtual void render() = 0;
	virtual void serialize(nlohmann::json& output_json) const = 0;
	virtual void deserialize(const nlohmann::json& input_json) = 0;

	friend void to_json(nlohmann::json& json, const Setting& setting) { setting.serialize(json); }
	friend void from_json(const nlohmann::json& json, Setting& setting) { setting.deserialize(json); }

	auto visible_condition(this auto&& self, std::function<void()> visible)
	{
		self.visible = std::move(visible);
		return self;
	}
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
