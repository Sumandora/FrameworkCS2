#include "Group.hpp"

#include "../Utils/GroupPanel.hpp"

namespace Menu {
	Group::Group(std::string name)
		: name(std::move(name))
	{
	}

	void Group::draw() const
	{
		GroupPanel::BeginGroupPanel(name.c_str());
		for (const auto& widget : widgets) {
			widget->draw();
		}
		GroupPanel::EndGroupPanel();
	}

	void Group::addWidget(std::unique_ptr<Widget> widget) { widgets.push_back(std::move(widget)); }
}