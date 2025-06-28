#pragma once

#include "../../Schema/FieldOffset.hpp"

#include "../../GameClass/SkeletonInstance.hpp"

#include "BodyComponent.hpp"

struct BodyComponentSkeletonInstance : public BodyComponent {
	CLASS_INFO("libclient.so", "CBodyComponentSkeletonInstance");

	SCHEMA_VAR(SkeletonInstance, skeleton_instance, "m_skeletonInstance");
};
