#pragma once

#include "../Schema/FieldOffset.hpp"
#include "Transform.hpp"

struct GameSceneNode {
	CLASS_INFO("libclient.so", "CGameSceneNode");

	SCHEMA_VAR(Transform, transform, "m_nodeToWorld");
};
