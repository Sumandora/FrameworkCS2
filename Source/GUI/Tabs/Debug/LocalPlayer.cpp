#include "../Tabs.hpp"

#include "../../../Memory.hpp"

#include "imgui.h"

void GUI::Tabs::Debug::draw_local_player()
{
	auto* local_player = Memory::local_player;
	if (local_player != nullptr)
		ImGui::Text("Local player: %p", static_cast<void*>(local_player));
	else
		ImGui::Text("No local player found!");
}
