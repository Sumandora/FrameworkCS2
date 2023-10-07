#pragma once

#include "../Schema/SchemaClassInfo.hpp"

struct BaseEntity {
	VIRTUAL_METHOD(35, getSchemaType, SchemaClassInfo*, (), (this));
};