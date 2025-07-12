#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <vector>

namespace VTexDecoder {
	struct RawImage {
		int width, height;
		struct Color {
			std::uint8_t r, g, b, a;
		};
		std::shared_ptr<Color[]> rgba;
	};

	// TODO Error type

	std::optional<RawImage> decode(std::vector<char> input_buffer);
}
