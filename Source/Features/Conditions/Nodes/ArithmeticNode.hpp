#pragma once

#include "../IdType.hpp"
#include "../Node.hpp"
#include "../NodeResult.hpp"
#include "../NodeType.hpp"

#include <cctype>
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
	IdType lhs{}, rhs{}, output{};
	ArithmeticOp operation;

public:
	explicit ArithmeticNode(NodeCircuit* parent, ArithmeticOp operation);
	~ArithmeticNode() override = default;

	void render_io() override;

	[[nodiscard]] NodeResult get_value() const override;

	[[nodiscard]] NodeType get_input_type(IdType /*id*/) const override
	{
		return NodeType::FLOAT;
	}
};
