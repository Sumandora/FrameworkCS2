#pragma once

#include "../Schema/FieldOffset.hpp"
#include "../Schema/SchemaClassInfo.hpp"
#include "Transform.hpp"

struct GameSceneNode {
	CLASS_INFO("libclient.so", "GameSceneNode");

	SCHEMA_VAR(Transform, transform, "m_nodeToWorld");
};