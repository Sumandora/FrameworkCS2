#pragma once

#include "../IdType.hpp"
#include "../Node.hpp"
#include "../NodeRegistry.hpp"
#include "../NodeResult.hpp"
#include "../NodeType.hpp"

#include <cstddef>
#include <cstdint>
#include <utility>

enum class RGBADirection : std::uint8_t {
	RGBA_TO_COLOR,
	COLOR_TO_RGBA,
};

class RGBANode : public Node {
	RGBADirection direction;

	IdType r, g, b, a;
	IdType color;

	explicit RGBANode(NodeCircuit* parent, RGBADirection direction, IdType r, IdType g, IdType b, IdType a, IdType color);

public:
	static RGBANode* initialized(NodeCircuit* parent, RGBADirection direction);
	static RGBANode* uninitialized(NodeCircuit* parent, RGBADirection direction);
	~RGBANode() override = default;

	void render_io() override;

	[[nodiscard]] NodeType get_input_type(IdType /*id*/) const override
	{
		switch (direction) {
		case RGBADirection::RGBA_TO_COLOR:
			return NodeType::FLOAT;
		case RGBADirection::COLOR_TO_RGBA:
			return NodeType::COLOR;
		}

		std::unreachable();
	}

	[[nodiscard]] NodeType get_output_type(IdType /*id*/) const override
	{
		switch (direction) {
		case RGBADirection::RGBA_TO_COLOR:
			return NodeType::COLOR;
		case RGBADirection::COLOR_TO_RGBA:
			return NodeType::FLOAT;
		}

		std::unreachable();
	}

	[[nodiscard]] NodeResult get_value(IdType /*id*/) const override;

	void serialize(nlohmann::json& output_json) const override;
	void deserialize(const nlohmann::json& input_json) override;
};
