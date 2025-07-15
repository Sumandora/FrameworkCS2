#pragma once

#include "../GUI/TextureManager.hpp"

#include <cstddef>
#include <optional>
#include <vector>

namespace VTexDecoder {
	// TODO Error type
	std::optional<GUI::TextureManager::RawImage> decode(std::vector<char> input_buffer);
}
