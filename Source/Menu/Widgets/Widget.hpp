#pragma once
#include <string>
#include <string_view>

namespace Menu {
	class Widget {
	public:
		explicit Widget(std::string name);
		virtual ~Widget() = default;
		virtual void draw() = 0;

		/**
		 * @brief Get the unique label of this widget.
		 *
		 * This is computationally free to use, the generation is only done once.
		 */
		[[nodiscard]] const std::string& getLabel() const;

	protected:
		std::string name;

	private:
		std::string label;
	};
}