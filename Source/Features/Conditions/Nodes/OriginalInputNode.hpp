#pragma once

#include "../Node.hpp"
#include "../IdType.hpp"
#include "../NodeResult.hpp"
#include "../NodeType.hpp"

#include <functional>

class OriginalInputNode : public Node {
	std::function<NodeResult()> get_original_value;

	IdType output = 0;
	NodeType type;

public:
	explicit OriginalInputNode(NodeCircuit* parent, NodeType type, std::function<NodeResult()> get_original_value);
	~OriginalInputNode() override = default;

	[[nodiscard]] NodeType get_input_type(IdType /*id*/) const override
	{
		return NodeType::NOTHING;
	}

	void render_io() override;
	[[nodiscard]] NodeResult get_value() const override;

	[[nodiscard]] IdType get_output() const { return output; }
};

