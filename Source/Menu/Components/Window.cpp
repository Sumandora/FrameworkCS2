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
		auto tab1 = std::make_unique<Tab2>("Test tab 1");
		Group group("Test group");
		group.addWidget(std::make_unique<ButtonArray>(
			std::vector<std::string>{ "100%", "125%", "150%", "200%" }, [](std::size_t index) {
				Config::c.menu.dpi.setValue(static_cast<Config::DPI>(index));
				updateFontDPI();
			}));
		tab1->addLeftGroup(std::move(group));
		auto tab2 = std::make_unique<Tab2>("Test tab 2");
		auto tab3 = std::make_unique<Tab2>("Test tab 3");
		addTab(std::move(tab1));
		addTab(std::move(tab2));
		addTab(std::move(tab3));
	}

	void Window::addTab(std::unique_ptr<Tab> tab)
	{
		buttonArray.addButton(tab->getName());
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

		ImGui::PushFont(fonts.menuBig);
		ImGui::TextUnformatted(name.c_str());
		ImGui::PopFont();

		ImGui::SameLine();
		buttonArray.draw();
		addSpacing(6);
		ImGui::Separator();
		tabs[buttonArray.getSelected()]->draw();

		ImGui::End();
	}
}