#pragma once

#include "../IdType.hpp"
#include "../Node.hpp"
#include "../NodeRegistry.hpp"
#include "../NodeResult.hpp"
#include "../NodeType.hpp"

#include "nlohmann/json_fwd.hpp"

#include <cctype>
#include <cstddef>
#include <cstdint>

enum class LogicGateType : std::uint8_t {
	// NOLINTBEGIN(readability-identifier-naming) -- magic_enum::enum_name
	And,
	Or,
	// NOLINTEND(readability-identifier-naming)
};

class LogicGateNode : public Node {
	LogicGateType type;

	IdType input_a{}, input_b{}, output{};

	explicit LogicGateNode(NodeCircuit* parent, LogicGateType type, IdType input_a, IdType input_b, IdType output);

public:
	static LogicGateNode* initialized(NodeCircuit* parent, LogicGateType type);
	static LogicGateNode* uninitialized(NodeCircuit* parent,LogicGateType type);
	~LogicGateNode() override = default;

	void render_io() override;

	[[nodiscard]] NodeResult get_value(IdType id) const override;

	[[nodiscard]] NodeType get_input_type(IdType /*id*/) const override { return NodeType::BOOLEAN; }
	[[nodiscard]] NodeType get_output_type(IdType /*id*/) const override { return NodeType::BOOLEAN; }

	void serialize(nlohmann::json& output_json) const override;
	void deserialize(const nlohmann::json& input_json) override;
};
