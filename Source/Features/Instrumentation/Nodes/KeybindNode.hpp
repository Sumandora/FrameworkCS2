#pragma once

#include "../IdType.hpp"
#include "../Node.hpp"
#include "../NodeRegistry.hpp"
#include "../NodeResult.hpp"
#include "../NodeType.hpp"

#include "../../../GUI/InputManager.hpp"

#include <cstddef>
#include <memory>

class KeybindNode : public Node {
	std::shared_ptr<GUI::InputManager::Key> key;
	IdType output{};

	explicit KeybindNode(NodeCircuit* parent, IdType output);

public:
	static KeybindNode* initialized(NodeCircuit* parent);
	static KeybindNode* uninitialized(NodeCircuit* parent);
	~KeybindNode() override = default;

	void render_io() override;

	[[nodiscard]] NodeType get_input_type(IdType /*id*/) const override { return NodeType::NOTHING; }
	[[nodiscard]] NodeType get_output_type(IdType /*id*/) const override { return NodeType::BOOLEAN; }

	[[nodiscard]] NodeResult get_value(IdType /*id*/) const override;

	void serialize(nlohmann::json& output_json) const override;
	void deserialize(const nlohmann::json& input_json) override;
};
