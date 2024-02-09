#include "Window.hpp"

#include <imgui.h>
#include <utility>

#include "../Menu.hpp"
#include "../Utils/Spacing.hpp"
#include "../Widgets/ColorPicker.hpp"
#include "../Widgets/ComboArray.hpp"
#include "../Widgets/Keybinder.hpp"
#include "../Widgets/TextInput.hpp"
#include "Tab.hpp"

#include <cmath>

namespace Menu {
	Window::Window(std::string name)
		: name(std::move(name))
	{
		tabSelector.setHighlightSelected(true);
		tabSelector.setWidth(80);
		auto legitTab = std::make_unique<Tab2>("Legit");
		auto rageTab = std::make_unique<Tab2>("Rage");
		auto visualsTab = std::make_unique<Tab2>("Visuals");
		addTab(std::move(legitTab));
		addTab(std::move(rageTab));
		addTab(std::move(visualsTab));

		auto miscTab = std::make_unique<Tab2>("\xef\x82\x85 Misc");
		Group menuGroup("Menu");
		menuGroup.addWidget(std::make_unique<ComboArray<Config::DPI>>("DPI", std::vector<std::string>{ "100%", "125%", "150%", "200%" },
			&Config::c.menu.dpi, std::optional{ []([[maybe_unused]] std::size_t index) { updateFontDPI(); } }));
		menuGroup.addWidget(std::make_unique<Keybinder>("Menu key", *Config::c.menu.openKey.getPointer()));
		menuGroup.addWidget(std::make_unique<ColorPicker>("Accent color", Config::c.menu.accentColor));
		miscTab->addLeftGroup(std::move(menuGroup));

		static Config::Setting<std::string> configName("config.json");
		Group configGroup("Config");
		configGroup.addWidget(std::make_unique<TextInput>("Config name", configName));
		configGroup.addWidget(std::make_unique<ButtonArray>("I/O",
			std::vector<std::pair<std::string, std::function<void()>>>{
				{ "Save", []() { Config::save(*configName); } },
				{ "Load", []() { Config::load(*configName); } } }));
		miscTab->addRightGroup(std::move(configGroup));

		addTab(std::move(miscTab));
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