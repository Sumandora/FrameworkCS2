#pragma once

#include "Widget.hpp"

#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace Menu {
	class ButtonArray final : public Widget {
	public:
		ButtonArray(std::string name, std::vector<std::string> names, bool highlightSelected,
			std::optional<std::function<void(std::size_t)>> callback,
			std::optional<float> width = std::nullopt);

		/**
		 * A simplified constructor for a button array which has the same semantics as `ImGui::Combo`
		 * i.e. the user selects one of multiple options and that selection is stored.
		 */
		ButtonArray(std::string name, std::vector<std::string> names);

		/**
		 * A simplified constructor for a button array which has the same semantics as n `ImGui::Button`s,
		 * i.e. each button represents a separate action, no state is stored.
		 */
		ButtonArray(std::string name, const std::vector<std::pair<std::string, std::function<void()>>>& actions);

		ButtonArray(std::string name, std::vector<std::string> names, std::function<void(std::size_t)> callback);

		void draw() override;
		[[nodiscard]] std::size_t getSelected() const;

		void addButton(std::string name);
		void setWidth(float width);
		void setHighlightSelected(bool highlightSelected);

	private:
		std::vector<std::string> names;
		bool highlightSelected;
		std::optional<std::function<void(std::size_t)>> callback;
		std::optional<float> width;
		std::size_t selected = 0;
	};
}