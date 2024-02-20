#pragma once

#include "../Schema/FieldOffset.hpp"
#include "../Schema/SchemaClassInfo.hpp"
#include "EconItemView.hpp"

struct AttributeContainer {
	CLASS_INFO("libclient.so", "C_AttributeContainer")

	SCHEMA_VAR(EconItemView, item, "m_Item")
};