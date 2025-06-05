#pragma once

#include "../IdType.hpp"
#include "../Node.hpp"
#include "../NodeResult.hpp"
#include "../NodeType.hpp"

class FloatValueNode : public Node {
	float value{};
	IdType output{};

public:
	explicit FloatValueNode(NodeCircuit* parent);
	~FloatValueNode() override = default;

	void render_io() override;

	[[nodiscard]] NodeType get_input_type(IdType /*id*/) const override
	{
		return NodeType::NOTHING;
	}

	[[nodiscard]] NodeResult get_value() const override
	{
		return NodeResult{ .f = value };
	}
};
