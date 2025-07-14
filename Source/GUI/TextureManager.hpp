#pragma once

#include "imgui.h"

#include "ImageLoader.hpp"

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <unordered_map>
#include <vector>

namespace GUI {
	class TextureManager {
	public:
		class RawImage {
		public:
			struct Color {
				std::uint8_t r, g, b, a;
			};
			static_assert(sizeof(Color) == sizeof(std::uint32_t));

		private:
			int width, height;
			std::vector<std::byte> compressed_rgba;
			std::size_t uncompressed_size;

		public:
			RawImage(int width, int height, const std::vector<Color>& rgba);

			[[nodiscard]] int get_width() const { return width; }
			[[nodiscard]] int get_height() const { return height; }

			[[nodiscard]] std::vector<std::uint32_t> get_rgba() const;
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
