#include "glm/ext/vector_float3.hpp"

#include <cstddef>

using namespace glm;

static_assert(offsetof(vec3, x) == 0);
static_assert(offsetof(vec3, y) == 4);
static_assert(offsetof(vec3, z) == 8);
static_assert(sizeof(vec3) == 12);

// NOLINTNEXTLINE -- yeah this is unsafe, but it's isolated in this translation unit
#define private public
#include "cs_usercmd.pb.h"
#include "usercmd.pb.h"

static_assert(sizeof(google::protobuf::Message) == 16);
static_assert(offsetof(CSGOUserCmdPB::Impl_, base_) == 32);
static_assert(offsetof(CBaseUserCmdPB::Impl_, viewangles_) == 48);
