#pragma once

#include "imgui.h"
#include "nlohmann/json.hpp"
#include "nlohmann/json_fwd.hpp"

#include <string>
#include <utility>

class Feature;

class Setting {
	std::string name;

public:
	explicit Setting(Feature* parent, std::string name);
	virtual ~Setting() = default;

	const std::string& get_name() { return name; }

	virtual void render() = 0;
	virtual void serialize(nlohmann::json& output_json) const = 0;
	virtual void deserialize(const nlohmann::json& input_json) = 0;

	friend void to_json(nlohmann::json& json, const Setting& setting)
	{
		setting.serialize(json);
	}
	friend void from_json(const nlohmann::json& json, Setting& setting)
	{
		setting.deserialize(json);
	}
};

class Checkbox : public Setting {
	bool value{};

public:
	Checkbox(Feature* parent, std::string name)
		: Setting(parent, std::move(name))
	{
	}

	[[nodiscard]] bool get() const { return value; }

	void render() override
	{
		ImGui::Checkbox(get_name().c_str(), &value);
	}

	void serialize(nlohmann::json& output_json) const override
	{
		output_json = value;
	}

	void deserialize(const nlohmann::json& input_json) override
	{
		value = input_json;
	}
};
