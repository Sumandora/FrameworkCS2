#pragma once

#include "../Setting.hpp"
#include "../Settings/MetaSetting.hpp"

#include "../../GUI/Elements/ResizablePopup.hpp"

#include "NodeCircuit.hpp"
#include "NodeResult.hpp"
#include "NodeType.hpp"

#include "imgui.h"
#include "imnodes.h"

#include <concepts>
#include <cstddef>
#include <memory>
#include <mutex>
#include <string>
#include <utility>

template <typename T>
	requires std::is_base_of_v<Setting, T>
class InstrumentableSetting : public MetaSetting {
	T setting;
	mutable std::mutex circuit_access;
	std::unique_ptr<NodeCircuit> node_circuit = nullptr;
	bool was_open = false;

public:
	template <typename... Args>
	explicit InstrumentableSetting(SettingsHolder* parent, std::string name, Args&&... args)
		: MetaSetting(parent, name)
		, setting(this, name, std::forward<Args>(args)...)
	{
	}

	auto get() const
	{
		const std::lock_guard<std::mutex> guard{ circuit_access };
		if (node_circuit)
			return unwrap_node_result<decltype(setting.get())>(node_circuit->get_output());
		return setting.get();
	}

	void make_node_circuit()
	{
		node_circuit = std::make_unique<NodeCircuit>(nodetype_for<decltype(setting.get())>(), [&s = this->setting] {
			return NodeResult::create(s.get());
		});
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
				make_node_circuit();
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

	void serialize(nlohmann::json& output_json) const override
	{
		setting.serialize(output_json["Main"]);
		const std::lock_guard<std::mutex> guard{ circuit_access };
		if (node_circuit && !node_circuit->is_trivial())
			node_circuit->serialize(output_json["Circuit"]);
	}

	void deserialize(const nlohmann::json& input_json) override
	{
		setting.deserialize(input_json["Main"]);
		if (input_json.contains("Circuit")) {
			const std::lock_guard<std::mutex> guard{ circuit_access };
			if (!node_circuit)
				make_node_circuit();

			node_circuit->deserialize(input_json["Circuit"]);
		}
	}
};
