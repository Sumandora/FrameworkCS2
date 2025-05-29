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

class FloatNumber : public Setting {
	float value = 0.0F, min, max;

public:
	FloatNumber(SettingsHolder* parent, std::string name, float min, float max)
		: Setting(parent, std::move(name))
		, min(min)
		, max(max)
	{
	}

	[[nodiscard]] float get() const { return value; }

	void render() override
	{
		ImGui::SliderFloat(get_name().c_str(), &value, min, max);
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

class Color : public Setting {
	ImColor value{ 1.0F, 1.0F, 1.0F, 1.0F };

public:
	Color(SettingsHolder* parent, std::string name)
		: Setting(parent, std::move(name))
	{
	}

	[[nodiscard]] ImColor get() const { return value; }

	void render() override
	{

		const bool clicked = ImGui::ColorButton((get_name() + "##Button").c_str(), value, ImGuiColorEditFlags_None, ImVec2(0, 0));
		ImGui::SameLine();
		ImGui::Text("%s", get_name().c_str());

		auto id = get_name() + "##Popup";
		if (clicked)
			ImGui::OpenPopup(id.c_str());

		if (ImGui::BeginPopup(id.c_str())) {
			float float_array[] = { value.Value.x, value.Value.y, value.Value.z, value.Value.w };
			if (ImGui::ColorPicker4("##Picker", float_array, 0)) {
				value.Value = ImVec4(float_array[0], float_array[1], float_array[2], float_array[3]);
			}
			ImGui::EndPopup();
		}
	}

	void serialize(nlohmann::json& output_json) const override
	{
		output_json = { value.Value.x, value.Value.y, value.Value.z, value.Value.w };
	}

	void deserialize(const nlohmann::json& input_json) override
	{
		value.Value.x = input_json[0];
		value.Value.y = input_json[1];
		value.Value.z = input_json[2];
		value.Value.w = input_json[3];
	}
};

class Button : public Setting {
	std::function<void()> action;

public:
	Button(SettingsHolder* parent, std::string name, std::function<void()> action)
		: Setting(parent, std::move(name))
		, action(std::move(action))
	{
	}

	void render() override
	{
		if (ImGui::Button(get_name().c_str()))
			action();
	}

	void serialize(nlohmann::json& /*output_json*/) const override
	{
	}

	void deserialize(const nlohmann::json& /*input_json*/) override
	{
	}
};

class Subgroup : public Setting, public SettingsHolder {
	bool anonymous;

public:
	Subgroup(SettingsHolder* parent, std::string name, bool anonymous)
		: Setting(parent, std::move(name))
		, anonymous(anonymous)
	{
	}

	void render() override
	{
		if (!anonymous)
			ImGui::Text("%s", get_name().c_str());

		ImGui::SameLine();

		const std::string popup_id = get_name() + "##Popup";
		if (ImGui::Button(("...##" + get_name()).c_str()))
			ImGui::OpenPopup(popup_id.c_str());

		if (ImGui::BeginPopup(popup_id.c_str())) {
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
