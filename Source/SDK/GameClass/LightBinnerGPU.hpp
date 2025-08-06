#pragma once

#include "../VirtualMethod.hpp"

struct SceneLightObject;

class LightBinnerGPU {
public:
	VIRTUAL_METHOD(3, process_lights, void, (SceneLightObject * scene_light_object, void* unk), (scene_light_object, unk));
};
