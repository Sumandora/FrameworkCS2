#pragma once

#include "../Feature.hpp"

#include "../Settings/Checkbox.hpp"

#include "../../Utils/UninitializedObject.hpp"

class UserCmd;

class Bhop : public Feature {
	Checkbox enabled{ this, "Enabled", false };

public:
	Bhop();

	void create_move(UserCmd* cmd);
};

inline UninitializedObject<Bhop> bhop;
