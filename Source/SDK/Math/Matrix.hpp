#pragma once

#include <cstdint>
#include <array>

template<std::size_t W, std::size_t H, typename T = float>
using Matrix = std::array<std::array<T, H>, W>;

using VMatrix = Matrix<4, 4>;