#pragma once

#include <string>

#include "../Schema/FieldOffset.hpp"
#include "../Schema/SchemaClassInfo.hpp"

struct EconItemView {
	CLASS_INFO("libclient.so", "C_EconItemView")

	const std::string& getName() const;

protected:
	SCHEMA_VAR(std::uint16_t, itemDefinitionIndex, "m_iItemDefinitionIndex")
};