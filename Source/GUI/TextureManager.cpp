#include "TextureManager.hpp"

#include "ImageLoader.hpp"

#include "imgui.h"

#include "../Utils/Logging.hpp"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <utility>

ImTextureID GUI::TextureManager::get_texture(const RawImage& image)
{
	if (auto it = textures.find(&image); it != textures.end()) {
		it->second.last_usage = std::chrono::system_clock::now();
		return reinterpret_cast<ImTextureID>(it->second.tex_data.descriptor_set);
	}

	const auto& timed_tex_data = textures[&image] = TimedTextureData{
		.last_usage = std::chrono::system_clock::now(),
		.tex_data = ImageLoader::create_texture(
			image.width,
			image.height,
			reinterpret_cast<std::uint32_t*>(image.rgba.get())),
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
