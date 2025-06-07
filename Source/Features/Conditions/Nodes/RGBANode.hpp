#pragma once

#include "../IdType.hpp"
#include "../Node.hpp"
#include "../NodeRegistry.hpp"
#include "../NodeResult.hpp"
#include "../NodeType.hpp"

#include <cstddef>
#include <cstdint>

enum class RGBADirection : std::uint8_t {
	FLOATS_TO_RGBA,
	RGBA_TO_FLOATS
};

class RGBANode : public Node {
	RGBADirection direction;

	IdType r, g, b, a;
	IdType color;

	explicit RGBANode(NodeCircuit* parent, RGBADirection direction, IdType r, IdType g, IdType b, IdType a, IdType color);

public:
	static RGBANode* initialized(NodeCircuit* parent, RGBADirection direction);
	static RGBANode* uninitialized(NodeCircuit* parent);
	~RGBANode() override = default;

	void render_io() override;

	[[nodiscard]] NodeType get_input_type(IdType /*id*/) const override {
		switch(direction) {
		case RGBADirection::FLOATS_TO_RGBA:
			return NodeType::FLOAT;
		case RGBADirection::RGBA_TO_FLOATS:
			return NodeType::COLOR_RGBA;
		}
	}
	[[nodiscard]] NodeType get_output_type(IdType /*id*/) const override {
		switch(direction) {
		case RGBADirection::FLOATS_TO_RGBA:
			return NodeType::COLOR_RGBA;
		case RGBADirection::RGBA_TO_FLOATS:
			return NodeType::FLOAT;
		}
	}

	[[nodiscard]] NodeResult get_value() const override;

	void serialize(nlohmann::json& output_json) const override;
	void deserialize(const nlohmann::json& input_json) override;
};
