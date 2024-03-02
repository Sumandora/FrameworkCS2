#include "Menu.hpp"

#include "../Menu/Widgets/ComboArray.hpp"
#include "../Menu/Widgets/Keybinder.hpp"
#include "../Menu/Widgets/ColorPicker.hpp"
#include "../Menu/Menu.hpp"

Features::Menu::Menu() : Feature("Menu") {

}


ImColor Features::Menu::getAccentColor() {
	return *accentColor;
}

Config::Key Features::Menu::getOpenKey() {
	return *openKey;
}

float Features::Menu::getDpiScale()
{
	switch (*dpi) {
		using enum DPI;
	case X1_25:
		return 1.25f;
	case X1_5:
		return 1.5f;
	case X2:
		return 2.0f;
	case X1: default:
		return 1.0f;
	}
}

void Features::Menu::imguiRender(::Menu::Group& group) {
	using namespace ::Menu;
	group.addWidget(std::make_unique<ComboArray<Features::Menu::DPI>>("DPI", std::vector<std::string>{ "100%", "125%", "150%", "200%" },
		&dpi, std::optional{ []([[maybe_unused]] std::size_t index) { updateFontDPI(); } }));
	group.addWidget(std::make_unique<Keybinder>("Menu key", *openKey.getPointer()));
	group.addWidget(std::make_unique<ColorPicker>("Accent color", accentColor));

}
