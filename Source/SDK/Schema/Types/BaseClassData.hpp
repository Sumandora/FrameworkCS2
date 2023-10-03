#pragma once

#include "../../Padding.hpp"
#include "../SchemaClassInfo.hpp"

struct BaseClassData {
	PADDING(8);
	SchemaClassInfo classInfo;
};