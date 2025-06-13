#pragma once

#include "../IdType.hpp"
#include "../Node.hpp"
#include "../NodeRegistry.hpp"
#include "../NodeResult.hpp"
#include "../NodeType.hpp"

#include <cstddef>
#include <functional>

class OriginalInputNode : public Node {
	std::function<NodeResult()> get_original_value;

	IdType output = 0;
	NodeType type;

public:
	explicit OriginalInputNode(NodeCircuit* parent, NodeType type, std::function<NodeResult()> get_original_value);
	~OriginalInputNode() override = default;

	void render_io() override;

	[[nodiscard]] NodeResult get_value(IdType id) const override;

	[[nodiscard]] NodeType get_input_type(IdType /*id*/) const override { return NodeType::NOTHING; }
	[[nodiscard]] NodeType get_output_type(IdType /*id*/) const override { return type; }
	[[nodiscard]] IdType get_output() const { return output; }

	void serialize(nlohmann::json& output_json) const override;
	void deserialize(const nlohmann::json& input_json) override;
};
