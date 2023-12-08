#include "Window.hpp"

#include <imgui.h>
#include <utility>

#include "../Menu.hpp"
#include "../Utils/Spacing.hpp"
#include "Tab.hpp"

#include <cmath>

namespace Menu {
	Window::Window(std::string name)
		: name(std::move(name))
	{
		tabSelector.setHighlightSelected(true);
		tabSelector.setWidth(80);
		tab1->addLeftGroup(std::move(group));
		auto tab2 = std::make_unique<Tab2>("Test tab 2");
		auto tab3 = std::make_unique<Tab2>("Test tab 3");
		addTab(std::move(tab1));
		addTab(std::move(tab2));
		addTab(std::move(tab3));
	}

	void Window::addTab(std::unique_ptr<Tab> tab)
	{
		tabSelector.addButton(tab->getName());
		tabs.push_back(std::move(tab));
	}

	void Window::draw()
	{
		ImGui::SetNextWindowSize({ 400, 300 }, ImGuiCond_Once);
		if (*Config::c.menu.dpi != lastDPI) {
			const float delta = getDpiScale(*Config::c.menu.dpi) / getDpiScale(lastDPI);
			lastSize.x *= delta;
			lastSize.y *= delta;
			lastSize.x = std::floor(lastSize.x);
			lastSize.y = std::floor(lastSize.y);
			lastDPI = *Config::c.menu.dpi;
			ImGui::SetNextWindowSize(lastSize, ImGuiCond_Always);
		}
		ImGui::Begin(name.c_str(), nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);

		addSpacingUnscaled(-7);
		addSpacing(7);

		ImGui::PushFont(fonts.menuBig);
		ImGui::TextUnformatted(name.c_str());
		ImGui::PopFont();

		ImGui::SameLine();
		buttonArray.draw();
		addSpacingUnscaled(4);
		addSpacing(3);
		ImGui::Separator();
		tabs[buttonArray.getSelected()]->draw();

		ImGui::End();
	}
}