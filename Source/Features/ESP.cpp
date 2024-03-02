#include "Features.hpp"

#include <limits>

#include "../Memory.hpp"
#include "../SDK/Entities/BaseEntity.hpp"
#include "../SDK/Entities/CSPlayerPawn.hpp"
#include "../SDK/Entities/CSPlayerPawnBase.hpp"
#include "../SDK/Entities/GameEntitySystem.hpp"

#include "../SDK/GameClass/CollisionProperty.hpp"
#include "../SDK/GameClass/GameSceneNode.hpp"
#include "../Utils/Projection.hpp"

#include "GenericESP.hpp"
#include "GenericESP/DefaultRenderers.hpp"

using namespace GenericESP;

static DefaultRenderers renderers;
RendererFactory& GenericESP::rendererFactory = renderers;

static struct EntityESP : ESP {
	Rectangle box{ this, "Box" };

	void draw(ImDrawList* drawList, const BaseEntity* e, const ImRect& rect) const
	{
		UnionedRect unionedRect{ rect };
		box.draw(drawList, e, unionedRect);
	}
} entityEsp;

Features::ESP::ESP() : Feature("ESP") {

}

void Features::ESP::drawEsp(ImDrawList* drawList)
{
	if (entityEsp.isDefinitelyDisabled())
		return;
	int highest = (*Memory::EntitySystem::gameEntitySystem)->getHighestEntityIndex();
	if (highest > -1)
		for (int i = 0; i <= highest; i++) {
			BaseEntity* entity = (*Memory::EntitySystem::gameEntitySystem)->getBaseEntity(i);
			if (entity == nullptr)
				continue;
			if (!entityEsp.isEnabled(entity))
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

			ImRect rectangle{
				{ std::numeric_limits<float>::max(), std::numeric_limits<float>::max() },
				{ std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest() }
			};

			for (const auto& point : points) {
				ImVec2 point2D;

				if (!Projection::project(point, point2D)) {
					goto next_ent;
				}

				if (point2D.x < rectangle.Min.x)
					rectangle.Min.x = point2D.x;
				else if (point2D.x > rectangle.Max.x)
					rectangle.Max.x = point2D.x;

				if (point2D.y < rectangle.Min.y)
					rectangle.Min.y = point2D.y;
				else if (point2D.y > rectangle.Max.y)
					rectangle.Max.y = point2D.y;
			}

			entityEsp.draw(drawList, entity, rectangle);
		next_ent:;
		}
}

void Features::ESP::imguiRender()
{
	if (ImGui::Begin("ESP")) {
		entityEsp.renderGui();
	}
	ImGui::End();
}

void Features::ESP::serialize(libconfig::Setting& settingsGroup) const {
	// TODO
}

void Features::ESP::deserialize(const libconfig::Setting& settingsGroup) {
	// TODO
}