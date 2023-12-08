#include "Window.hpp"

#include <imgui.h>
#include <utility>

#include "../Menu.hpp"
#include "../Utils/Spacing.hpp"
#include "../Widgets/ComboArray.hpp"
#include "../Widgets/Keybinder.hpp"
#include "Tab.hpp"

#include <cmath>

namespace Menu {
	Window::Window(std::string name)
		: name(std::move(name))
	{
		tabSelector.setHighlightSelected(true);
		tabSelector.setWidth(80);
		auto tab1 = std::make_unique<Tab2>("\xef\x82\x85 Misc");
		Group group("Menu");
		group.addWidget(std::make_unique<ComboArray<Config::DPI>>("DPI", std::vector<std::string>{ "100%", "125%", "150%", "200%" },
			&Config::c.menu.dpi, std::optional{[]([[maybe_unused]] std::size_t index) { updateFontDPI(); }}));
		group.addWidget(std::make_unique<Keybinder>("Menu key", Config::c.menu.openKey.getPointer()));
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
		ImGui::SetNextWindowSize(lastSize, ImGuiCond_Once);
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
		tabSelector.draw();
		addSpacingUnscaled(4);
		addSpacing(3);
		ImGui::Separator();
		tabs[tabSelector.getSelected()]->draw();

		lastSize = ImGui::GetWindowSize();
		ImGui::End();
	}
}