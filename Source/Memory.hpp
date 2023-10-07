#pragma once

struct ViewRender;
struct VMatrix;

namespace Memory {
	inline ViewRender* viewRender;
	inline VMatrix* worldToProjectionMatrix;

	void Create();
}