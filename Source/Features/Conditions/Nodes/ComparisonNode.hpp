#pragma once

#include "../IdType.hpp"
#include "../Node.hpp"
#include "../NodeResult.hpp"
#include "../NodeType.hpp"

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <string>

enum class ComparisonOp : std::uint8_t {
	// NOLINTBEGIN(readability-identifier-naming) -- magic_enum::enum_name
	Equals,
	Not_equals,
	Lower_than,
	Greater_than,
	Lower_or_equal_than,
	Greater_or_equal_than,
	// NOLINTEND(readability-identifier-naming)
};

constexpr std::string replace_underscores_with_spaces(std::string underscores) {
	std::ranges::replace(underscores, '_', ' ');
	return underscores;
}

class ComparisonNode : public Node {
	IdType lhs{}, rhs{}, output{};
	ComparisonOp operation;

public:
	explicit ComparisonNode(NodeCircuit* parent, ComparisonOp operation);
	~ComparisonNode() override = default;

	void render_io() override;

	[[nodiscard]] NodeResult get_value() const override;

	[[nodiscard]] NodeType get_input_type(IdType /*id*/) const override
	{
		return NodeType::FLOAT;
	}
};
