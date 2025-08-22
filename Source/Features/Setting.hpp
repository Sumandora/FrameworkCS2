#pragma once

#include <cstring>
#include <functional>
#include <string>

#include "gch/small_vector.hpp"

#include "nlohmann/json.hpp" // IWYU pragma: keep

class Setting;

class SettingsHolder {
protected:
	// Since BindableSettings all are technically setting-holders as well and the most common case
	// is that there are no binds, I think a small vector makes sense here.
	// NOLINTNEXTLINE(cppcoreguidelines-non-private-member-variables-in-classes)
	gch::small_vector<Setting*, 1> settings;

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
	// TODO Rename to set_visible_condition, since you can't have multiple
	void add_visible_condition(std::function<bool()> visible) &;
};
