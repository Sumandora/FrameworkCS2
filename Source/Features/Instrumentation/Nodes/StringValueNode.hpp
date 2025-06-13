#pragma once

#include "../IdType.hpp"
#include "../Node.hpp"
#include "../NodeRegistry.hpp"
#include "../NodeResult.hpp"
#include "../NodeType.hpp"

#include <cstddef>
#include <string>

class StringValueNode : public Node {
	std::string value;
	IdType output{};

	explicit StringValueNode(NodeCircuit* parent, IdType output);

public:
	static StringValueNode* initialized(NodeCircuit* parent);
	static StringValueNode* uninitialized(NodeCircuit* parent);
	~StringValueNode() override = default;

	void render_io() override;

	[[nodiscard]] NodeType get_input_type(IdType /*id*/) const override { return NodeType::NOTHING; }
	[[nodiscard]] NodeType get_output_type(IdType /*id*/) const override { return NodeType::STRING; }

	[[nodiscard]] NodeResult get_value(IdType /*id*/) const override { return value; }

	void serialize(nlohmann::json& output_json) const override;
	void deserialize(const nlohmann::json& input_json) override;
};
