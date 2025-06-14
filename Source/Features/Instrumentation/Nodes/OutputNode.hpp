#pragma once

#include "../IdType.hpp"
#include "../Node.hpp"
#include "../NodeRegistry.hpp"
#include "../NodeResult.hpp"
#include "../NodeType.hpp"

#include <cstddef>

class NodeCircuit;

class OutputNode : public Node {
	IdType input = 0;
	NodeType type;

public:
	explicit OutputNode(NodeCircuit* parent, NodeType type);
	~OutputNode() override = default;

	void render_io() override;
	[[nodiscard]] NodeResult get_value(IdType id) const override;

	[[nodiscard]] NodeType get_input_type(IdType /*id*/) const override { return type; }
	[[nodiscard]] NodeType get_output_type(IdType /*id*/) const override { return NodeType::NOTHING; }

	[[nodiscard]] IdType get_input() const { return input; }

	void serialize(nlohmann::json& output_json) const override;
	void deserialize(const nlohmann::json& output_json) override;
};
