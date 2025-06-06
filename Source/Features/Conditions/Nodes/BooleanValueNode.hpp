#pragma once

#include "../IdType.hpp"
#include "../Node.hpp"
#include "../NodeRegistry.hpp"
#include "../NodeResult.hpp"
#include "../NodeType.hpp"

#include <cstddef>

class BooleanValueNode : public Node {
	bool value{};
	IdType output{};

	explicit BooleanValueNode(NodeCircuit* parent, IdType output);

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

	[[nodiscard]] std::size_t node_id() const override;

	void serialize(nlohmann::json& output_json) const override;
	static BooleanValueNode* deserialize(NodeCircuit* parent, const nlohmann::json& input_json);
};
