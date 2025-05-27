#pragma once

#include "imgui.h"
#include "nlohmann/json.hpp"
#include "nlohmann/json_fwd.hpp"

#include <string>
#include <utility>
#include <vector>

class Setting;

class SettingsHolder {
protected:
	std::vector<Setting*> settings;

	friend Setting;
};

class Setting {
	std::string name;

public:
	explicit Setting(SettingsHolder* parent, std::string name);
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
	Checkbox(SettingsHolder* parent, std::string name)
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

class Subgroup : public Setting, public SettingsHolder {
public:
	Subgroup(SettingsHolder* parent, std::string name)
		: Setting(parent, std::move(name))
	{
	}

	void render() override
	{
		static const std::string POPUP_LABEL = get_name() + "##Popup";

		ImGui::Text("%s", get_name().c_str());

		ImGui::SameLine();
		
		if (ImGui::Button("..."))
			ImGui::OpenPopup(POPUP_LABEL.c_str());

		if (ImGui::BeginPopup(POPUP_LABEL.c_str())) {
			for (Setting* setting : settings)
				setting->render();

			ImGui::EndPopup();
		}
	}

	void serialize(nlohmann::json& output_json) const override
	{
		for (Setting* setting : settings)
			setting->serialize(output_json[setting->get_name()]);
	}

	void deserialize(const nlohmann::json& input_json) override
	{
		for (Setting* setting : settings)
			setting->deserialize(input_json[setting->get_name()]);
	}

	// NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
	operator SettingsHolder*() { return this; }
};
