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

	[[nodiscard]] NodeType get_input_type(IdType /*id*/) const override
	{
		return type;
	}

	void render_io() override;
	[[nodiscard]] NodeResult get_value() const override;

	[[nodiscard]] IdType get_input() const { return input; }

	[[nodiscard]] std::size_t node_id() const override;

	void serialize(nlohmann::json& output_json) const override;
};
