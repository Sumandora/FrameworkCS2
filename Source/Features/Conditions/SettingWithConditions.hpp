#pragma once

#include "../Setting.hpp"
#include "../Settings/MetaSetting.hpp"

#include "../../GUI/Elements/ResizablePopup.hpp"

#include "NodeCircuit.hpp"
#include "NodeResult.hpp"
#include "NodeType.hpp"

#include "imgui.h"
#include "imnodes.h"
#include "imnodes_internal.h"

#include "../../Utils/Logging.hpp"

#include <concepts>
#include <cstddef>
#include <memory>
#include <mutex>
#include <string>
#include <utility>

template <typename T>
	requires std::is_base_of_v<Setting, T>
class SettingWithConditions : public MetaSetting {
	T setting;
	mutable std::mutex circuit_access;
	std::unique_ptr<NodeCircuit> node_circuit = nullptr;
	bool was_open = false;

public:
	template <typename... Args>
	explicit SettingWithConditions(SettingsHolder* parent, std::string name, Args... args)
		: MetaSetting(parent, "##Conditions_" + name)
		, setting(this, name, args...)
	{
	}

	auto get() const
	{
		const std::lock_guard<std::mutex> guard{ circuit_access };
		if (node_circuit)
			return unwrap_node_result<decltype(setting.get())>(node_circuit->get_output());
		return setting.get();
	}

	void render() override
	{
		setting.render();

		ImGui::PushID(this);
		ImGui::OpenPopupOnItemClick("Conditions");

		ImGui::SetNextWindowSize(ImVec2{ 400, 300 }, ImGuiCond_Once);
		if (ImGuiExt::BeginResizablePopup("Conditions")) {
			// This lock is being held for an extended amount of time here, however it is only while rendering.
			// `get` will be called while in game, so they shouldn't intersect that much.
			const std::lock_guard<std::mutex> guard{ circuit_access };
			if (!node_circuit)
				node_circuit = std::make_unique<NodeCircuit>(NodeTypeFor<decltype(setting.get())>::TYPE, [&s = this->setting] {
					if constexpr (std::same_as<decltype(s.get()), bool>) {
						return NodeResult{ .b = s.get() };
					} else if constexpr (std::same_as<decltype(s.get()), float>) {
						return NodeResult{ .f = s.get() };
					}
					std::unreachable();
				});
			bool newly_opened = !was_open;
			was_open = true;

			ImGui::PushID("Circuit");
			node_circuit->render(newly_opened);
			ImGui::PopID();
			ImGui::EndPopup();
		} else if (was_open) {
			const std::lock_guard<std::mutex> guard{ circuit_access };
			if (node_circuit->is_trivial())
				node_circuit = nullptr;
			was_open = false;
		}
		ImGui::PopID();
	}

	// TODO x2
	void serialize(nlohmann::json& output_json) const override
	{
		setting.serialize(output_json["Main"]);
		// auto& conditions = output_json["Conditions"];
		// for (const auto& [cond, elem] : this->conditions) {
		// 	nlohmann::json j;
		// 	j["Condition"] = cond;
		// 	j["Element"] = elem;
		// 	conditions.push_back(j);
		// }
	}

	void deserialize(const nlohmann::json& input_json) override
	{
		setting.deserialize(input_json["Main"]);
		// if (input_json.contains("Conditions")) {
		// 	const auto& conditions = input_json["Conditions"];
		// 	for (const auto& j : conditions) {
		// 		auto& p = this->conditions.emplace_back(j["Condition"].get<Condition>(), factory());
		// 		p.second.deserialize(j["Element"]);
		// 	}
		// }
	}
};
