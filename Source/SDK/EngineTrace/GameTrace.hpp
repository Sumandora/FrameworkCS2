#pragma once

#include "../Padding.hpp"

#include "glm/ext/vector_float3.hpp"

#include <cstddef>

struct BaseEntity;

struct GameTrace {
	PADDING(8);
	BaseEntity* hit_entity;
    PADDING(103);
    glm::vec3 from;
    glm::vec3 to;
    PADDING(32);
    float fraction;
    PADDING(7);
    bool unk; // I think this indicates if the trace hit something...
    PADDING(1);

    static GameTrace initialized();
};

static_assert(sizeof(GameTrace) == 0xc0);
