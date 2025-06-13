#pragma once

#include "../Setting.hpp"
#include "../Settings/MetaSetting.hpp"

#include "../../GUI/Elements/ResizablePopup.hpp"

#include "NodeCircuit.hpp"
#include "NodeResult.hpp"
#include "NodeType.hpp"

#include "imgui.h"
#include "imnodes.h"

#include <cstddef>
#include <memory>
#include <mutex>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>

template <typename T, typename... AdditionalNodes>
	requires std::is_base_of_v<Setting, T>
class InstrumentableSetting : public MetaSetting {
	T setting;
	mutable std::mutex circuit_access;
	std::unique_ptr<NodeCircuit> node_circuit = nullptr;
	bool was_open = false;

	using GetType = std::remove_cvref_t<decltype(setting.get())>;
	static constexpr NodeType NODE_TYPE = nodetype_for<GetType>();
	static constexpr bool GIVES_REF = std::is_reference_v<decltype(setting.get())>;

	// Eh, this is annoying, but somewhat needed to prevent unneeded copies.
	[[no_unique_address]] mutable std::conditional_t<GIVES_REF, GetType, std::monostate> last_result;

public:
	template <typename... Args>
	explicit InstrumentableSetting(SettingsHolder* parent, std::string name, Args&&... args)
		: MetaSetting(parent, name)
		, setting(this, name, std::forward<Args>(args)...)
	{
	}

	decltype(setting.get()) get() const
	{
		const std::lock_guard<std::mutex> guard{ circuit_access };
		if (node_circuit) {
			NodeResult node_result = node_circuit->get_output();
			if (node_result.full()) {
				if constexpr (GIVES_REF) {
					last_result = node_result.get<GetType>();
					return last_result;
				} else
					return node_result.get<GetType>();
			}
			// TODO inform the user that the node circuit didn't evaluate.
		}
		return setting.get();
	}

	void make_node_circuit()
	{
		node_circuit = std::make_unique<NodeCircuit>(NODE_TYPE, [&s = this->setting] {
			return s.get();
		});
		(node_circuit->get_node_registry().add_node_by_type<AdditionalNodes>(), ...);
	}

	void render() override
	{
		setting.render();

		ImGui::PushID(this);
		ImGui::OpenPopupOnItemClick("Instrumentation");

		ImGui::SetNextWindowSize(ImVec2{ 400, 300 }, ImGuiCond_Once);
		if (ImGuiExt::BeginResizablePopup("Instrumentation")) {
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
