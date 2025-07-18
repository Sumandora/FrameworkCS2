#pragma once

#include "../Schema/FieldOffset.hpp"

#include "PointEntity.hpp"

struct MapInfo : public PointEntity {
	CLASS_INFO("libclient.so", "CMapInfo");

	SCHEMA_VAR(float, bomb_radius, "m_flBombRadius");
};
