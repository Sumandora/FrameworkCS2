#include "Features.hpp"

#include <limits>

#include "../Memory.hpp"
#include "../SDK/Entities/BaseEntity.hpp"
#include "../SDK/Entities/GameEntitySystem.hpp"
#include "../SDK/Entities/Player/CSPlayerPawn.hpp"
#include "../SDK/Services/CSPlayerWeaponServices.hpp"

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

	UnionedRect draw(ImDrawList* drawList, const BaseEntity* e, const ImRect& rect) const
	{
		UnionedRect unionedRect{ rect };
		box.draw(drawList, e, unionedRect);
		return unionedRect;
	}
} entityEsp;

static struct PlayerESP : ESP {
	Rectangle box{ this, "Box" };
	Bar healthBar{ this, "Health bar", [](const CSPlayerPawn* pawn) { return static_cast<float>(pawn->health()) / 100.0f; } };
	Bar ammoBar{ this, "Ammo bar", [](const CSPlayerPawn* pawn) {
					const auto weapon = pawn->weaponServices()->activeWeapon().get();
					if (weapon == nullptr)
						return 0.0f;
					const auto vdata = weapon->getWeaponVData();
					if (vdata == nullptr)
						return 0.0f;
					return static_cast<float>(weapon->clip1()) / static_cast<float>(vdata->maxClip1());
				} };
	Bar armorBar{ this, "Armor bar", [](const CSPlayerPawn* pawn) { return static_cast<float>(pawn->armorValue()) / 100.0f; } };
	SidedText name{ this, "Name", Side::TOP };
	SidedText weapon{ this, "Weapon", Side::BOTTOM };

	UnionedRect draw(ImDrawList* drawList, const CSPlayerPawn* e, const ImRect& rect) const
	{
		UnionedRect unionedRect{ rect };
		box.draw(drawList, e, unionedRect);
		healthBar.draw(drawList, e, unionedRect);
		ammoBar.draw(drawList, e, unionedRect);
		armorBar.draw(drawList, e, unionedRect);
    const auto controller = e->originalController();
    if (controller.isValid()) {
      const auto nameStr = std::string{ controller.get()->sanitizedPlayerName() };
      name.draw(drawList, e, nameStr, unionedRect);
    }
		const auto activeWeapon = e->weaponServices()->activeWeapon().get();
		if (activeWeapon != nullptr) {
			weapon.draw(drawList, e, activeWeapon->attributeManager().item().getName(), unionedRect);
		}
		return unionedRect;
	}
} playerEsp;

void Features::ESP::drawEsp(ImDrawList* drawList)
{
	if (playerEsp.isDefinitelyDisabled())
		return;
	int highest = Memory::gameEntitySystem->getHighestEntityIndex();
	if (highest > -1)
		for (int i = 0; i <= highest; i++) {
			BaseEntity* entity = Memory::gameEntitySystem->getBaseEntity(i);
			if (entity == nullptr)
				continue;
			if (!playerEsp.isEnabled(entity))
				continue;
			auto schemaType = entity->getSchemaType();
			if (schemaType != CSPlayerPawn::classInfo())
				continue;
			auto* player = static_cast<CSPlayerPawn*>(entity);

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

			rectangle.Min.x = std::round(rectangle.Min.x);
			rectangle.Min.y = std::round(rectangle.Min.y);
			rectangle.Max.x = std::round(rectangle.Max.x);
			rectangle.Max.y = std::round(rectangle.Max.y);

			playerEsp.draw(drawList, player, rectangle);
		next_ent:;
		}
}

void Features::ESP::imguiRender()
{
	if (ImGui::Begin("ESP")) {
		playerEsp.renderGui();
	}
	ImGui::End();
}
