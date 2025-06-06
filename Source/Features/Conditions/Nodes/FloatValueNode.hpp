#pragma once

#include "../IdType.hpp"
#include "../Node.hpp"
#include "../NodeRegistry.hpp"
#include "../NodeResult.hpp"
#include "../NodeType.hpp"

#include <cstddef>

class FloatValueNode : public Node {
	float value{};
	IdType output{};

	explicit FloatValueNode(NodeCircuit* parent, IdType output);

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

	[[nodiscard]] std::size_t node_id() const override;

	void serialize(nlohmann::json& output_json) const override;
	static FloatValueNode* deserialize(NodeCircuit* parent, const nlohmann::json& input_json);
};
