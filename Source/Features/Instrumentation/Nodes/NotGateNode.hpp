#pragma once

#include "../IdType.hpp"
#include "../Node.hpp"
#include "../NodeRegistry.hpp"
#include "../NodeResult.hpp"
#include "../NodeType.hpp"

#include "nlohmann/json_fwd.hpp"

#include <cctype>
#include <cstddef>

class NotGateNode : public Node {
	IdType input{}, output{};

	explicit NotGateNode(NodeCircuit* parent, IdType input, IdType output);

public:
	static NotGateNode* initialized(NodeCircuit* parent);
	static NotGateNode* uninitialized(NodeCircuit* parent);
	~NotGateNode() override = default;

	void render_io() override;

	[[nodiscard]] NodeResult get_value(IdType id) const override;

	[[nodiscard]] NodeType get_input_type(IdType /*id*/) const override { return NodeType::BOOLEAN; }
	[[nodiscard]] NodeType get_output_type(IdType /*id*/) const override { return NodeType::BOOLEAN; }

	void serialize(nlohmann::json& output_json) const override;
	void deserialize(const nlohmann::json& input_json) override;
};
