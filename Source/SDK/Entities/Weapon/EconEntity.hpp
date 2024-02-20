#pragma once

#include "../../GameClass/AttributeContainer.hpp"
#include "../../Schema/FieldOffset.hpp"
#include "../../Schema/SchemaClassInfo.hpp"
#include "../BaseEntity.hpp"

struct EconEntity : BaseEntity {
	CLASS_INFO("libclient.so", "C_EconEntity")

	SCHEMA_VAR(AttributeContainer, attributeManager, "m_AttributeManager")
};