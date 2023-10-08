#pragma once

#include "../Schema/SchemaClassInfo.hpp"
#include "../Schema/FieldOffset.hpp"
#include "CTransform.hpp"

struct CGameSceneNode {
	CLASS_INFO("libclient.so", "CGameSceneNode");

	SCHEMA_VAR(CTransform, transform, "m_nodeToWorld");
};