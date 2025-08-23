#pragma once

#include "../../Feature.hpp"

#include "../../Settings/Checkbox.hpp"
#include "../../Settings/Tabs.hpp"

#include "../../../Utils/UninitializedObject.hpp"

#include "glm/ext/vector_float3.hpp"
#include "Inferno.hpp"

#include "imgui.h"

#include "Player.hpp"
#include "Projectile.hpp"
#include "Weapon.hpp"

#include <atomic>
#include <string>

struct CSPlayerPawn;
struct SchemaClassInfo;
struct BaseCSGrenadeProjectile;

class ESP : public Feature {
	Checkbox enabled{ this, "Enabled", false };

	Tabs entity_types{ this, "Entity types" };

	Tabs players{ entity_types, "Players" };
	ESPPlayer enemies{ players, "Enemies" };
	ESPPlayer teammates{ players, "Teammates" };
	ESPPlayer local{ players, "Local" };

	ESPWeapon weapons{ entity_types, "Weapons" };

	Tabs projectiles{ entity_types, "Projectiles" };
	// @see GrenadeType
	ESPProjectile hegrenade{ projectiles, "High Explosive Grenade", "#SFUI_WPNHUD_HE_Grenade" };
	ESPProjectile flashbang{ projectiles, "Flashbang", "#SFUI_WPNHUD_Flashbang" };
	ESPProjectile molotov{ projectiles, "Molotov", "#SFUI_WPNHUD_Molotov" };
	ESPProjectile incgrenade{ projectiles, "Incendiary Grenade", "#SFUI_WPNHUD_IncGrenade" };
	ESPProjectile decoy{ projectiles, "Decoy", "#SFUI_WPNHUD_Decoy" };
	ESPProjectile smokegrenade{ projectiles, "Smoke Grenade", "#SFUI_WPNHUD_Smoke_Grenade" };
	// TODO sensor grenades and snowballs? :^)

	// This tab seems appropriate for the more meme-y or niche esp targets, like chickens or infernos
	Tabs misc{ entity_types, "Misc" };
	ESPInferno inferno{ misc, "Inferno" };

	std::atomic<glm::vec3> camera_position;

	friend ESPPlayer;

	ESPPlayer& get_player_by_pawn(CSPlayerPawn* player_pawn);
	ESPProjectile& get_projectile_by_entity_and_class(BaseCSGrenadeProjectile* projectile, SchemaClassInfo* class_info);

public:
	ESP();

	void update_camera_position(const glm::vec3& new_camera_position);

	void draw(ImDrawList* draw_list);
};

inline UninitializedObject<ESP> esp;
