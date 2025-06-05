#pragma once

#include "../IdType.hpp"
#include "../Node.hpp"
#include "../NodeResult.hpp"
#include "../NodeType.hpp"

class BooleanValueNode : public Node {
	bool value{};
	IdType output{};

public:
	explicit BooleanValueNode(NodeCircuit* parent);
	~BooleanValueNode() override = default;

	void render_io() override;

	[[nodiscard]] NodeType get_input_type(IdType /*id*/) const override
	{
		return NodeType::NOTHING;
	}

	[[nodiscard]] NodeResult get_value() const override
	{
		return NodeResult{ .b = value };
	}
};
