#pragma once

#include "../Schema/FieldOffset.hpp"

#include "GameSceneNode.hpp"

#include "ModelState.hpp"

struct SkeletonInstance : public GameSceneNode {
public:
	CLASS_INFO("libclient.so", "CSkeletonInstance");

	SCHEMA_VAR(ModelState, model_state, "m_modelState");
};
