#pragma once

#include <cstring>
#include <functional>
#include <string>
#include <vector>

#include "nlohmann/json.hpp" // IWYU pragma: keep

class Setting;

class SettingsHolder {
protected:
	// NOLINTNEXTLINE(cppcoreguidelines-non-private-member-variables-in-classes)
	std::vector<Setting*> settings;

	friend Setting;

public:
	void render_all_childs();

	void serialize(nlohmann::json& output_json) const;
	void deserialize(const nlohmann::json& input_json);

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
