#pragma once

#include "IdType.hpp"
#include "NodeRegistry.hpp"
#include "NodeResult.hpp"
#include "NodeType.hpp"

#include "imgui.h"

#include "nlohmann/json_fwd.hpp"

#include <cstddef>
#include <optional>
#include <string>
#include <utility>

class NodeCircuit;

class Node {
	std::string name;
	NodeCircuit* parent;

	std::optional<ImVec2> queued_position;

public:
	explicit Node(NodeCircuit* parent);
	virtual ~Node() = default;

	virtual void render_io() = 0;

	void render_node(IdType id);

	[[nodiscard]] virtual NodeResult get_value() const = 0;

	[[nodiscard]] virtual NodeType get_input_type(IdType id) const = 0;
	[[nodiscard]] virtual NodeType get_output_type(IdType id) const = 0;

	void set_name(std::string name) { this->name = std::move(name); }
	[[nodiscard]] const std::string& get_name() const { return name; }

	[[nodiscard]] NodeCircuit* get_parent() const { return parent; }

	virtual void serialize(nlohmann::json& output_json) const = 0;
	virtual void deserialize(const nlohmann::json& input_json) = 0;

	void queue_position(ImVec2 position) { queued_position = position; }
};
