#pragma once

#include "imgui.h"

#include <cstdint>
#include <cstddef>

namespace GUI::ImageLoader {
	ImTextureID create_texture(std::size_t width, std::size_t height, uint32_t* rgba);
	void destroy_texture(ImTextureID texture);
}
