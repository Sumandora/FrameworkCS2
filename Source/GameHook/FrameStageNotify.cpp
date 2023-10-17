#include "GameHook.hpp"

#include <limits>

#include "../GraphicsHook/GraphicsHook.hpp"
#include "../SDK/Entities/GameEntitySystem.hpp"
#include "../SDK/GameClass/CollisionProperty.hpp"
#include "../SDK/GameClass/CSPlayerPawn.hpp"
#include "../SDK/GameClass/GameSceneNode.hpp"
#include "../Utils/Projection.hpp"
#include "imgui.h"

void drawEsp(ImDrawList* drawList)
{
	int highest = (*Memory::EntitySystem::gameEntitySystem)->getHighestEntityIndex();
	if (highest > -1)
		for (int i = 0; i <= highest; i++) {
			BaseEntity* entity = (*Memory::EntitySystem::gameEntitySystem)->getBaseEntity(i);
			if (entity == nullptr)
				continue;
			auto schemaType = entity->getSchemaType();
			if (schemaType != CSPlayerPawn::classInfo())
				continue;

			auto* gameSceneNode = entity->gameSceneNode();
			auto* transform = gameSceneNode->transformPtr();
			if (transform == nullptr)
				continue;
			Vector3 vec = transform->m_Position;

			auto* collision = entity->collision();
			if (collision == nullptr)
				continue;
			Vector3 mins = collision->mins();
			Vector3 maxs = collision->maxs();

			Vector3 finalMins = { vec[0] + mins[0], vec[1] + mins[1], vec[2] + mins[2] };
			Vector3 finalMaxs = { vec[0] + maxs[0], vec[1] + maxs[1], vec[2] + maxs[2] };

			const Vector3 points[] = {
				// Lower
				Vector3{ finalMins[0], finalMins[1], finalMins[2] },
				Vector3{ finalMaxs[0], finalMins[1], finalMins[2] },
				Vector3{ finalMaxs[0], finalMins[1], finalMaxs[2] },
				Vector3{ finalMins[0], finalMins[1], finalMaxs[2] },
				// Higher
				Vector3{ finalMins[0], finalMaxs[1], finalMins[2] },
				Vector3{ finalMaxs[0], finalMaxs[1], finalMins[2] },
				Vector3{ finalMaxs[0], finalMaxs[1], finalMaxs[2] },
				Vector3{ finalMins[0], finalMaxs[1], finalMaxs[2] }
			};

			Vector4 rectangle{ std::numeric_limits<float>::max(), std::numeric_limits<float>::max(),
				std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest() };

			for (const auto& point : points) {
				ImVec2 point2D;

				if (!project(point, point2D)) {
					goto next_ent;
				}

				if (point2D.x < rectangle[0])
					rectangle[0] = point2D.x;
				else if (point2D.x > rectangle[2])
					rectangle[2] = point2D.x;

				if (point2D.y < rectangle[1])
					rectangle[1] = point2D.y;
				else if (point2D.y > rectangle[3])
					rectangle[3] = point2D.y;
			}

			drawList->AddRect(ImVec2{ rectangle[0], rectangle[1] }, ImVec2{ rectangle[2], rectangle[3] }, ImColor(255, 255, 255, 255), 2.0f, 0, 5.0f);
		next_ent:
		}
}

void GameHook::FrameStageNotify::hookFunc([[maybe_unused]] void* thisptr, ClientFrameStage stage)
{
	switch (stage) {
		using enum ClientFrameStage;
	case FRAME_NET_UPDATE_END:
		const std::lock_guard<std::mutex> lock(GraphicsHook::espMutex);
		if (GraphicsHook::espDrawList != nullptr) { // it was not yet initialized by the other thread
			GraphicsHook::espDrawList->_ResetForNewFrame();
			GraphicsHook::espDrawList->PushClipRectFullScreen();
			GraphicsHook::espDrawList->PushTextureID(ImGui::GetIO().Fonts->TexID);
			drawEsp(GraphicsHook::espDrawList);
		}
		break;
	}
	return reinterpret_cast<Func>(hook->proxy)(thisptr, stage);
}
