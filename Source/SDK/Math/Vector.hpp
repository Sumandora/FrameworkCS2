#pragma once

#include <cstdint>
#include <array>

template <std::size_t N, typename T = float>
using Vector = std::array<T, N>;

using Vector2 = Vector<2>;
using Vector3 = Vector<3>;
using Vector4 = Vector<4>;