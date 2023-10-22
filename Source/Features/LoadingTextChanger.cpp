#include "Features.hpp"

#include "imgui_stdlib.h"

const char* Features::LoadingTextChanger::getReplacement()
{
	if(enabled)
		return replacement.c_str();
	return nullptr;
}

void Features::LoadingTextChanger::imguiRender()
{
	if (ImGui::Begin("Loading text changer")) {
		ImGui::Checkbox("Enabled", &enabled);

		ImGui::InputText("Replacement", &replacement);
	}
	ImGui::End();
}