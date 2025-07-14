#include "TextureManager.hpp"

#include "ImageLoader.hpp"

#include "imgui.h"

#include "../Utils/Logging.hpp"

#include "lz4.h"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <utility>
#include <vector>

GUI::TextureManager::RawImage::RawImage(int width, int height, const std::vector<Color>& rgba)
	: width(width)
	, height(height)
	, uncompressed_size(rgba.size() * sizeof(Color))
{
	compressed_rgba.resize(LZ4_compressBound(static_cast<int>(rgba.size() * sizeof(Color))));

	const int compressed_rgba_size = LZ4_compress_default(
		reinterpret_cast<const char*>(rgba.data()), reinterpret_cast<char*>(compressed_rgba.data()),
		static_cast<int>(rgba.size() * sizeof(Color)), static_cast<int>(compressed_rgba.size()));

	if (compressed_rgba_size <= 0)
		Logging::error("Failed to compress image data"); // TODO unsure how to handle this case, perhaps there should be a std::variant in case the compression isn't possible...

	compressed_rgba.resize(compressed_rgba_size); // Cut off unneeded bytes...
	compressed_rgba.shrink_to_fit();
}

std::vector<std::uint32_t> GUI::TextureManager::RawImage::get_rgba() const
{
	std::vector<std::uint32_t> uncompressed(uncompressed_size / sizeof(std::uint32_t) /* unsafe, but meh, should always work */);

	const int uncompressed_size = LZ4_decompress_safe(
		reinterpret_cast<const char*>(compressed_rgba.data()), reinterpret_cast<char*>(uncompressed.data()),
		static_cast<int>(compressed_rgba.size()), static_cast<int>(uncompressed.size() * sizeof(std::uint32_t)));

	if (uncompressed_size <= 0)
		Logging::error("Failed to decompress image data"); // TODO how to handle this?

	return uncompressed;
}

ImTextureID GUI::TextureManager::get_texture(const RawImage& image)
{
	if (auto it = textures.find(&image); it != textures.end()) {
		it->second.last_usage = std::chrono::system_clock::now();
		return reinterpret_cast<ImTextureID>(it->second.tex_data.descriptor_set);
	}

	std::vector<std::uint32_t> uncompressed_rgba = image.get_rgba();

	const auto& timed_tex_data = textures[&image] = TimedTextureData{
		.last_usage = std::chrono::system_clock::now(),
		.tex_data = ImageLoader::create_texture(
			image.get_width(),
			image.get_height(),
			uncompressed_rgba.data()),
	};
	return reinterpret_cast<ImTextureID>(timed_tex_data.tex_data.descriptor_set);
}

void GUI::TextureManager::purge_old_textures()
{
	if (textures.empty())
		return;

	static constexpr auto GLOBAL_COOLDOWN = std::chrono::seconds(3);
	static auto last_purge_time = std::chrono::system_clock::now();
	// If many textures have been created at the same time, then deallocating them all at once, might slow down the game.
	// This is not a perfect solution, I might need to investigate how other game engines handle this. TODO
	if (std::chrono::system_clock::now() - last_purge_time < GLOBAL_COOLDOWN)
		return;

	auto it = std::ranges::find_if(textures, [](const std::pair<const RawImage*, TimedTextureData>& pair) {
		static constexpr auto TEXTURE_PURGE_TIME = std::chrono::seconds(30);
		if (std::chrono::system_clock::now() - pair.second.last_usage <= TEXTURE_PURGE_TIME)
			return false;

		ImageLoader::destroy_texture(pair.second.tex_data);
		return true;
	});

	if (it == textures.end())
		return;

	textures.erase(it);
	last_purge_time = std::chrono::system_clock::now();
}

GUI::TextureManager& GUI::get_texture_manager()
{
	static GUI::TextureManager texture_manager{};
	return texture_manager;
}
