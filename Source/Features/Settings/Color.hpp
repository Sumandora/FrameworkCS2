#pragma once

#include "../Instrumentation/InstrumentableSetting.hpp"
#include "../Setting.hpp"

#include "imgui.h"

#include <mutex>
#include <shared_mutex>
#include <string>

class RawColor : public Setting {
	ImColor value;
	mutable std::shared_mutex mutex;

public:
	RawColor(SettingsHolder* parent, std::string name, ImColor value = { 1.0F, 1.0F, 1.0F, 1.0F });

	[[nodiscard]] ImColor get() const
	{
		const std::shared_lock lock{ mutex };
		return value;
	}
	void set(ImColor new_value)
	{
		const std::unique_lock lock{ mutex };
		value = new_value;
	}

	void render() override;
	void serialize(nlohmann::json& output_json) const override;
	void deserialize(const nlohmann::json& input_json) override;
};

using Color = InstrumentableSetting<RawColor>;
