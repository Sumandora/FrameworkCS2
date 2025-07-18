#pragma once

#include "../EntityHandle.hpp"
#include "../GameClass/GameTime.hpp"
#include "../Schema/FieldOffset.hpp"
#include "BaseAnimGraph.hpp"
#include "CSPlayerPawn.hpp"

#include <cstdint>

struct PlantedC4 : public BaseAnimGraph {
	CLASS_INFO("libclient.so", "C_PlantedC4");

	SCHEMA_VAR(bool, bomb_ticking, "m_bBombTicking");
	SCHEMA_VAR(std::uint32_t, bomb_site, "m_nBombSite");
	SCHEMA_VAR(GameTime, c4_blow, "m_flC4Blow");
	SCHEMA_VAR(bool, has_exploded, "m_bHasExploded");
	SCHEMA_VAR(float, defuse_length, "m_flDefuseLength");
	SCHEMA_VAR(GameTime, defuse_count_down, "m_flDefuseCountDown");
	SCHEMA_VAR(bool, defused, "m_bBombDefused");
	SCHEMA_VAR(bool, being_defused, "m_bBeingDefused");
	SCHEMA_VAR(EntityHandle<CSPlayerPawn>, bomb_defuser, "m_hBombDefuser");
};
