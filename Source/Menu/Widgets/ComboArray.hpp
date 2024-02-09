#pragma once

#include "../../Config/Setting.hpp"
#include "ButtonArray.hpp"
#include "Widget.hpp"

#include <imgui.h>
#include <string>
#include <type_traits>
#include <vector>

namespace Menu {
	template <typename T>
		requires std::is_enum_v<T>
	class ComboArray final : public Widget {
	public:
		ComboArray(std::string name, std::vector<std::string> items, Config::Setting<T>* setting,
			std::optional<std::function<void(std::size_t)>> callback = std::nullopt)
			: Widget(name)
			, array(std::move(name), std::move(items), true,
				  [setting, callback = std::move(callback)](std::size_t index) {
					  setting->setValue(static_cast<T>(index));
					  if (callback) {
						  callback.value()(index);
					  }
				  })
			, setting(setting)
		{
		}

		void draw() override
		{
			const auto yPadding = ImGui::GetStyle().FramePadding.y;
			const auto lineStart = ImGui::GetCursorPos();
			ImGui::Dummy(ImGui::CalcTextSize(name.c_str()));
			ImGui::SameLine();

			array.draw();

			ImGui::SetCursorPosX(lineStart.x);
			ImGui::SetCursorPosY(lineStart.y + yPadding);
			ImGui::TextUnformatted(name.c_str());
		}

	private:
		ButtonArray array;
		Config::Setting<T>* setting;
	};
}