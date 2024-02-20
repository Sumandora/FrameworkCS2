#pragma once

struct VMatrix;
struct ViewRender;
class GameEntitySystem;
struct BaseEntity;

namespace Memory {
	inline ViewRender* viewRender;
	inline VMatrix* worldToProjectionMatrix;
	inline GameEntitySystem* gameEntitySystem;

	inline void* shouldShowCrosshair;
	inline void* getLocalPlayer;

	inline void* fireEvent;

	void Create();
}