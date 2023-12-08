#include "Tab.hpp"

#include <imgui.h>

namespace Menu {
	Tab::Tab(std::string name)
		: name(std::move(name))
	{
	}

	std::string Tab::getName() const
	{
		return name;
	}

	Tab2::Tab2(std::string name)
		: Tab(std::move(name))
	{
	}

	void Tab2::addLeftGroup(Group group)
	{
		leftGroups.push_back(std::move(group));
	}

	void Tab2::addRightGroup(Group group)
	{
		rightGroups.push_back(std::move(group));
	}

	void Tab2::draw() const
	{
		ImGui::Columns(2, nullptr, false);
		const auto width = ImGui::GetWindowContentRegionMax().x / 2.f;
		ImGui::SetColumnWidth(0, width);
		ImGui::SetColumnWidth(1, width);
		for (const auto& group : leftGroups) {
			group.draw();
		}
		ImGui::NextColumn();
		for (const auto& group : rightGroups) {
			group.draw();
		}
	}
}