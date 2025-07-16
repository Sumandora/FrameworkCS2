#pragma once

#include "../VirtualMethod.hpp"
#include "../Padding.hpp"
#include "glm/ext/vector_float3.hpp"

struct UserCmd;

class CSGOInput {
public:
	VIRTUAL_METHOD(6, create_move, void*, (int unk1, char unk2), (this, unk1, unk2));
	VIRTUAL_METHOD(8, sync_view_angles, void, (int rsi), (this, rsi));

	OFFSET(bool, third_person, 0x261);
	OFFSET(glm::vec3, third_person_angles, 0x268);
	OFFSET(glm::vec3, view_angles, 0x3E0); // Found inside sync view angles function
};
