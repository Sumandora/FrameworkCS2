#pragma once

#include "SDK/Math/Matrix.hpp"
struct ViewRender;
struct GameEntitySystem;
struct BaseEntity;

namespace Memory {
	inline ViewRender* viewRender;
	inline VMatrix* worldToProjectionMatrix;

	namespace EntitySystem { // TODO Reorganize the entire structure of this
		inline ::GameEntitySystem** gameEntitySystem;
		inline void* getHighestEntityIndex;
		inline void* getBaseEntity;
	}

	inline void* shouldShowCrosshair;
	inline void* getLocalPlayer;

	inline void* fireEvent;

	void Create();
}