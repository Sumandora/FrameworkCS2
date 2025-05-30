#include "../Tabs.hpp"

#include "../../../SDK/Entities/BaseEntity.hpp"
#include "../../../SDK/Entities/CSPlayerPawn.hpp"
#include "../../../SDK/Entities/GameEntitySystem.hpp"
#include "../../../SDK/GameClass/CollisionProperty.hpp"
#include "../../../SDK/GameClass/GameSceneNode.hpp"

#include "glm/ext/vector_float3.hpp"

#include "imgui.h"

void GUI::Tabs::Debug::draw_entity_list()
{
	const int highest = GameEntitySystem::the()->getHighestEntityIndex();
	ImGui::Text("%d entities", highest);
	static bool only_players = false;
	ImGui::Checkbox("Only players", &only_players);
	if (highest > -1)
		if (ImGui::BeginTable("Entity List", 4, ImGuiTableFlags_SizingStretchProp)) {
			for (int i = 0; i <= highest; i++) {
				BaseEntity* entity = GameEntitySystem::the()->getBaseEntity(i);
				if (entity == nullptr) {
					if (!only_players) {
						ImGui::TableNextColumn();
						ImGui::Text("null entity?");
						ImGui::TableNextColumn();
						ImGui::TableNextColumn();
						ImGui::TableNextColumn();
					}
					continue;
				}
				auto* schema_type = entity->getSchemaType();
				if (only_players && schema_type != CSPlayerPawn::classInfo())
					continue;
				ImGui::TableNextColumn();
				ImGui::Text("%d", i);
				ImGui::TableNextColumn();
				ImGui::Text("%p", static_cast<void*>(entity));
				ImGui::TableNextColumn();
				ImGui::Text("%s", schema_type->className);
				ImGui::TableNextColumn();
				auto* game_scene_node = entity->gameSceneNode();
				auto& transform = game_scene_node->transform();
				glm::vec3 vec = transform.m_Position;

				auto* collision = entity->collision();
				if (collision == nullptr)
					continue;
				glm::vec3 mins = collision->mins();
				glm::vec3 maxs = collision->maxs();

				glm::vec3 final_mins = { vec[0] + mins[0], vec[1] + mins[1], vec[2] + mins[2] };
				glm::vec3 final_maxs = { vec[0] + maxs[0], vec[1] + maxs[1], vec[2] + maxs[2] };

				ImGui::Text("(%f %f %f) - (%f %f %f)",
					final_mins[0], final_mins[1], final_mins[2],
					final_maxs[0], final_maxs[1], final_maxs[2]);
			}

			ImGui::EndTable();
		}
}
