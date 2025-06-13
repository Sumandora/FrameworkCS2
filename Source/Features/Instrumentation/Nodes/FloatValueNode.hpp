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
	static FloatValueNode* initialized(NodeCircuit* parent);
	static FloatValueNode* uninitialized(NodeCircuit* parent);
	~FloatValueNode() override = default;

	void render_io() override;

	[[nodiscard]] NodeType get_input_type(IdType /*id*/) const override { return NodeType::NOTHING; }
	[[nodiscard]] NodeType get_output_type(IdType /*id*/) const override { return NodeType::FLOAT; }

	[[nodiscard]] NodeResult get_value(IdType /*id*/) const override { return value; }

	void serialize(nlohmann::json& output_json) const override;
	void deserialize(const nlohmann::json& input_json) override;
};
