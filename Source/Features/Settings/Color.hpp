#pragma once

#include "../Setting.hpp"
#include "../Instrumentation/InstrumentableSetting.hpp"

#include "imgui.h"

#include <atomic>
#include <string>

class RawColor : public Setting {
	std::atomic<ImColor> value;

public:
	RawColor(SettingsHolder* parent, std::string name, ImColor value = { 1.0F, 1.0F, 1.0F, 1.0F });

	[[nodiscard]] ImColor get() const { return value.load(std::memory_order::relaxed); }

	void render() override;
	void serialize(nlohmann::json& output_json) const override;
	void deserialize(const nlohmann::json& input_json) override;
};

using Color = InstrumentableSetting<RawColor>;
