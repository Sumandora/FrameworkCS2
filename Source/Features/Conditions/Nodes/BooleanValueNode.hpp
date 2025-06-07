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
	static BooleanValueNode* initialized(NodeCircuit* parent);
	static BooleanValueNode* uninitialized(NodeCircuit* parent);
	~BooleanValueNode() override = default;

	void render_io() override;

	[[nodiscard]] NodeType get_input_type(IdType /*id*/) const override { return NodeType::NOTHING; }
	[[nodiscard]] NodeType get_output_type(IdType /*id*/) const override { return NodeType::BOOLEAN; }

	[[nodiscard]] NodeResult get_value(IdType /*id*/) const override { return { .b = value }; }

	void serialize(nlohmann::json& output_json) const override;
	void deserialize(const nlohmann::json& input_json) override;
};
