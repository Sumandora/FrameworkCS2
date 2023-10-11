#pragma once

#include "../Math/Matrix.hpp"
#include "../Math/Vector.hpp"

class alignas(16) Transform {
public:
	alignas(16) Vector3 m_Position;
	alignas(16) Vector4 m_Orientation;

	[[nodiscard]] Matrix<3, 4> toMatrix() const;
};
