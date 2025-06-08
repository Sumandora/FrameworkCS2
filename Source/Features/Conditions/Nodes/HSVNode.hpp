#pragma once

#include "../IdType.hpp"
#include "../Node.hpp"
#include "../NodeRegistry.hpp"
#include "../NodeResult.hpp"
#include "../NodeType.hpp"

#include <cstddef>
#include <cstdint>
#include <utility>

enum class HSVDirection : std::uint8_t {
	HSVA_TO_COLOR,
	COLOR_TO_HSVA,
};

class HSVNode : public Node {
	HSVDirection direction;

	IdType h, s, v, a;
	IdType color;

	explicit HSVNode(NodeCircuit* parent, HSVDirection direction, IdType h, IdType s, IdType v, IdType a, IdType color);

public:
	static HSVNode* initialized(NodeCircuit* parent, HSVDirection direction);
	static HSVNode* uninitialized(NodeCircuit* parent);
	~HSVNode() override = default;

	void render_io() override;

	[[nodiscard]] NodeType get_input_type(IdType /*id*/) const override
	{
		switch (direction) {
		case HSVDirection::HSVA_TO_COLOR:
			return NodeType::FLOAT;
		case HSVDirection::COLOR_TO_HSVA:
			return NodeType::COLOR;
		}

		std::unreachable();
	}

	[[nodiscard]] NodeType get_output_type(IdType /*id*/) const override
	{
		switch (direction) {
		case HSVDirection::HSVA_TO_COLOR:
			return NodeType::COLOR;
		case HSVDirection::COLOR_TO_HSVA:
			return NodeType::FLOAT;
		}

		std::unreachable();
	}

	[[nodiscard]] NodeResult get_value(IdType /*id*/) const override;

	void serialize(nlohmann::json& output_json) const override;
	void deserialize(const nlohmann::json& input_json) override;
};
