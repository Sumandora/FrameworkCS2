#pragma once

#include "../IdType.hpp"
#include "../Node.hpp"
#include "../NodeRegistry.hpp"
#include "../NodeResult.hpp"
#include "../NodeType.hpp"

#include "nlohmann/json_fwd.hpp"

#include <cctype>
#include <cstddef>
#include <cstdint>

enum class ArithmeticOp : std::uint8_t {
	// NOLINTBEGIN(readability-identifier-naming) -- magic_enum::enum_name
	Addition,
	Subtraction,
	Multiplication,
	Division,
	Power,
	// NOLINTEND(readability-identifier-naming)
};

class ArithmeticNode : public Node {
	ArithmeticOp operation;

	IdType lhs{}, rhs{}, output{};

	explicit ArithmeticNode(NodeCircuit* parent, ArithmeticOp operation, IdType lhs, IdType rhs, IdType output);

public:
	static ArithmeticNode* initialized(NodeCircuit* parent,ArithmeticOp operation);
	static ArithmeticNode* uninitialized(NodeCircuit* parent);
	~ArithmeticNode() override = default;

	void render_io() override;

	[[nodiscard]] NodeResult get_value() const override;

	[[nodiscard]] NodeType get_input_type(IdType /*id*/) const override
	{
		return NodeType::FLOAT;
	}

	void serialize(nlohmann::json& output_json) const override;
	void deserialize(const nlohmann::json& input_json) override;
};
