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
	FLOATS_TO_HSV,
	HSV_TO_FLOATS
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
		case HSVDirection::FLOATS_TO_HSV:
			return NodeType::FLOAT;
		case HSVDirection::HSV_TO_FLOATS:
			return NodeType::COLOR;
		}

		std::unreachable();
	}

	[[nodiscard]] NodeType get_output_type(IdType /*id*/) const override
	{
		switch (direction) {
		case HSVDirection::FLOATS_TO_HSV:
			return NodeType::COLOR;
		case HSVDirection::HSV_TO_FLOATS:
			return NodeType::FLOAT;
		}

		std::unreachable();
	}

	[[nodiscard]] NodeResult get_value(IdType /*id*/) const override;

	void serialize(nlohmann::json& output_json) const override;
	void deserialize(const nlohmann::json& input_json) override;
};
