#pragma once

#include "../BindableSetting.hpp"
#include "../Instrumentation/IdType.hpp"
#include "../Instrumentation/Node.hpp"
#include "../Instrumentation/NodeCircuit.hpp"
#include "../Instrumentation/NodeResult.hpp"
#include "../Instrumentation/NodeType.hpp"
#include "../Setting.hpp"

#include "imgui.h"
#include "imnodes.h"

#include "magic_enum/magic_enum.hpp"

#include "../../GUI/Elements/EnumCombo.hpp"

#include <string>
#include <string_view>
#include <utility>

template <typename E>
	requires std::is_enum_v<E>
class RawCombo : public Setting {
	E value;
	// TODO thread-safety?

public:
	RawCombo(SettingsHolder* parent, std::string name, E value = magic_enum::enum_values<E>()[0])
		: Setting(parent, std::move(name))
		, value(value)
	{
	}

	[[nodiscard]] E get() const { return value; }
	void set(E new_value) { value = new_value; }

	void render() override
	{
		ImGuiExt::EnumCombo(get_name().c_str(), value);
	}
	void serialize(nlohmann::json& output_json) const override
	{
		output_json = std::to_underlying(value);
	}
	void deserialize(const nlohmann::json& input_json) override
	{
		value = static_cast<E>(input_json);
	}
};

template <typename E>
	requires std::is_enum_v<E>
class EnumNode : public Node {
	E value{};
	IdType output{};

	explicit EnumNode(NodeCircuit* parent, IdType output)
		: Node(parent)
		, output(output)
	{
	}

public:
	static constexpr std::string_view NAME = "Primitives/Enum value";
	static EnumNode* initialized(NodeCircuit* parent)
	{
		return new EnumNode{ parent, parent->next_id() };
	}
	static EnumNode* uninitialized(NodeCircuit* parent)
	{
		return new EnumNode{ parent, 0 };
	}
	~EnumNode() override = default;

	void render_io() override
	{
		ImGui::SetNextItemWidth(100.0F);
		ImGuiExt::EnumCombo("Value", value);

		ImGui::Spacing();

		ImNodes::BeginOutputAttribute(output);
		ImGui::TextUnformatted("output");
		ImNodes::EndOutputAttribute();
	}

	[[nodiscard]] NodeType get_input_type(IdType /*id*/) const override { return NodeType::NOTHING; }
	[[nodiscard]] NodeType get_output_type(IdType /*id*/) const override { return NodeType::ENUM; }

	[[nodiscard]] NodeResult get_value(IdType /*id*/) const override { return value; }

	void serialize(nlohmann::json& output_json) const override
	{
		output_json["value"] = std::to_underlying(value);
		output_json["output"] = output;
	}

	void deserialize(const nlohmann::json& input_json) override
	{
		value = magic_enum::enum_cast<E>(input_json["value"]).value();
		output = input_json["output"];
	}
};

template <typename E>
using Combo = BindableSetting<RawCombo<E>>;
