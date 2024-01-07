#pragma once
#include <string>
#include <string_view>

namespace Menu {
	class Widget {
	public:
		Widget();
		virtual ~Widget() = default;
		virtual void draw() = 0;

		/**
		 * @brief Get the unique label of this widget.
		 *
		 * This is free to use, the generation is only done once.
		 */
		[[nodiscard]] const std::string& getLabel() const;

	private:
		std::string label;
	};
}