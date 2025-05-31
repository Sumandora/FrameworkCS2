#include "../Tabs.hpp"

#include <dlfcn.h>
#include <link.h>

#include "imgui.h"

// NOLINTNEXTLINE(misc-no-recursion)
static void construct_inner(link_map* link_map)
{
	if (ImGui::TreeNode(link_map->l_name[0] == '\0' ? "Root" : link_map->l_name)) {
		ImGui::Text("Address: %p", reinterpret_cast<void*>(link_map->l_addr));
		ImGui::Text("Dynamic section: %p", reinterpret_cast<void*>(link_map->l_ld));

		if (link_map->l_prev)
			construct_inner(link_map->l_prev);
		if (link_map->l_next)
			construct_inner(link_map->l_next);
		ImGui::TreePop();
	}
}

void GUI::Tabs::Debug::draw_link_maps()
{
	void* handle = dlmopen(LM_ID_BASE, nullptr, RTLD_NOW | RTLD_NOLOAD);
	auto* link_map = reinterpret_cast<struct link_map*>(handle);
	while (link_map->l_prev)
		link_map = link_map->l_prev;
	dlclose(handle);

	while (link_map) {
		construct_inner(link_map);
		link_map = link_map->l_next;
	}
}
