#pragma once

#include "../Schema/FieldOffset.hpp"

#include "../CUtl/SymbolLarge.hpp"

struct Model;

struct ModelState {
	CLASS_INFO("libclient.so", "CModelState");

	SCHEMA_VAR(UtlSymbolLarge, model_name, "m_ModelName");
	SCHEMA_VAR(Model*, model, "m_hModel");
};
