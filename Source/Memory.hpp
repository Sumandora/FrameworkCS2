#pragma once

#include "SDK/Math/Matrix.hpp"
struct ViewRender;

namespace Memory {
	inline ViewRender* viewRender;
	inline VMatrix* worldToProjectionMatrix;

	void Create();
}