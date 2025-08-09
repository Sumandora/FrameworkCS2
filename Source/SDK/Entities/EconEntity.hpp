#pragma once

#include "BaseEntity.hpp"

#include "../Schema/FieldOffset.hpp"

#include "../Econ/AttributeContainer.hpp"

struct EconEntity : public BaseEntity {
	CLASS_INFO("libclient.so", "C_EconEntity");

	SCHEMA_VAR(AttributeContainer, attribute_manager, "m_AttributeManager");
};
