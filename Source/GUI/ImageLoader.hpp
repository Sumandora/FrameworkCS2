#pragma once

#include "vulkan/vulkan.h" // IWYU pragma: keep

#include <cstddef>
#include <cstdint>

namespace GUI::ImageLoader {
	struct TextureData {
		VkDescriptorSet descriptor_set;

		VkImageView image_view;
		VkImage image;
		VkDeviceMemory image_memory;
		VkSampler sampler;
		VkBuffer upload_buffer;
		VkDeviceMemory upload_buffer_memory;
	};

	TextureData create_texture(std::size_t width, std::size_t height, uint32_t* rgba);
	void destroy_texture(const TextureData& tex_data);
}
