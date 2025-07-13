#pragma once

#include "imgui.h"

#include "ImageLoader.hpp"

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <unordered_map>

namespace GUI {
	class TextureManager {
	public:
		struct RawImage {
			int width, height;
			struct Color {
				std::uint8_t r, g, b, a;
			};
			std::shared_ptr<Color[]> rgba;
		};

	private:
		struct TimedTextureData {
			std::chrono::system_clock::time_point last_usage;
			ImageLoader::TextureData tex_data;
		};
		std::unordered_map<const RawImage*, TimedTextureData> textures;

	public:
		// Images are identified by their pointer, don't move them around. TODO
		ImTextureID get_texture(const RawImage& image);

		// Purges textures that haven't been used in a while.
		void purge_old_textures();
	};

	TextureManager& get_texture_manager();
}
