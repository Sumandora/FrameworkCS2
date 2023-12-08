#pragma once

#include <string>
#include <vector>

#include "Group.hpp"

namespace Menu {
	class Tab {
	public:
		explicit Tab(std::string name);
		virtual ~Tab() = default;

		virtual void draw() const = 0;
		[[nodiscard]] std::string getName() const;

	private:
		std::string name;
	};

	class Tab2 final : public Tab {
	public:
		explicit Tab2(std::string name);
		void addLeftGroup(Group group);
		void addRightGroup(Group group);
		void draw() const override;

	private:
		std::vector<Group> leftGroups;
		std::vector<Group> rightGroups;
	};
}