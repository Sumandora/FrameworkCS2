#pragma once

#include "../Padding.hpp"

#include "GameSceneNode.hpp"

#include "glm/ext/matrix_float2x4.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/ext/vector_float4.hpp"

#include <cstdint>

enum BoneIds : std::uint8_t {
	BONE_HEAD = 6,
	// TODO add more
};

struct SkeletonInstance : GameSceneNode {
public:
	PADDING(0x1CC);
	std::uint32_t bone_count;
	PADDING(0x20);
	struct Bone { // I think this is a matrix, but this is more clear.
		glm::vec3 pos;
		float scale;
		glm::vec4 rotation;
	};
	static_assert(sizeof(glm::mat2x4) == sizeof(Bone));
	Bone* bones;
};
