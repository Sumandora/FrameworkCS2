#pragma once

#include "../IdType.hpp"
#include "../Node.hpp"
#include "../NodeRegistry.hpp"
#include "../NodeResult.hpp"
#include "../NodeType.hpp"

#include "nlohmann/json_fwd.hpp"

#include <cctype>
#include <cstddef>

class BranchNode : public Node {
	IdType falsy, condition, truthy, output;

	explicit BranchNode(NodeCircuit* parent, IdType falsy, IdType condition, IdType truthy, IdType output);

public:
	static BranchNode* initialized(NodeCircuit* parent);
	static BranchNode* uninitialized(NodeCircuit* parent);
	~BranchNode() override = default;

	void render_io() override;

	[[nodiscard]] NodeResult get_value(IdType id) const override;

	[[nodiscard]] NodeType get_input_type(IdType id) const override;
	[[nodiscard]] NodeType get_output_type(IdType /*id*/) const override;

	void serialize(nlohmann::json& output_json) const override;
	void deserialize(const nlohmann::json& input_json) override;
};
