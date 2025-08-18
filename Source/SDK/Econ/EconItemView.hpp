#pragma once

#include "../Schema/FieldOffset.hpp"

#include "../VirtualMethod.hpp"

#include <cstdint>

struct EconItemDefinition;

struct EconItemView {
	CLASS_INFO("libclient.so", "C_EconItemView");

	SCHEMA_VAR(std::uint16_t, item_definition_index, "m_iItemDefinitionIndex");

	VIRTUAL_METHOD(14, get_item_definition, EconItemDefinition*, () const, (this));
};
