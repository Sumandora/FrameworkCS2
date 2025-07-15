#include "ImageLoader.hpp"

#include "backends/imgui_impl_vulkan.h"

#include "../Hooks/Graphics/VulkanHook.hpp"

#include <cstddef>
#include <cstdint>
#include <cstring>

#include <vulkan/vulkan.h>

// https://github.com/ocornut/imgui/wiki/Image-Loading-and-Displaying-Examples#example-for-vulkan-users

static std::uint32_t find_memory_type(std::uint32_t type_filter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties mem_properties;
	vkGetPhysicalDeviceMemoryProperties(g_PhysicalDevice, &mem_properties);

	for (std::uint32_t i = 0; i < mem_properties.memoryTypeCount; i++)
		if ((type_filter & (1 << i)) && (mem_properties.memoryTypes[i].propertyFlags & properties) == properties)
			return i;

	return 0xFFFFFFFF;
}

auto GUI::ImageLoader::create_texture(std::size_t width, std::size_t height, uint32_t* rgba) -> TextureData
{
	const std::size_t channels = 4; // RGBA

	const std::size_t image_size = width * height * channels;

	TextureData tex_data;

	{
		const VkImageCreateInfo info{
			.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.imageType = VK_IMAGE_TYPE_2D,
			.format = VK_FORMAT_R8G8B8A8_UNORM,
			.extent = {
				.width = static_cast<uint32_t>(width),
				.height = static_cast<uint32_t>(height),
				.depth = 1,
			},
			.mipLevels = 1,
			.arrayLayers = 1,
			.samples = VK_SAMPLE_COUNT_1_BIT,
			.tiling = VK_IMAGE_TILING_OPTIMAL,
			.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
			.queueFamilyIndexCount = 0,
			.pQueueFamilyIndices = nullptr,
			.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		};
		CHECK_VK_ERR << vkCreateImage(g_Device, &info, g_Allocator, &tex_data.image);

		VkMemoryRequirements req;
		vkGetImageMemoryRequirements(g_Device, tex_data.image, &req);

		const VkMemoryAllocateInfo alloc_info{
			.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
			.pNext = nullptr,
			.allocationSize = req.size,
			.memoryTypeIndex = find_memory_type(req.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
		};
		CHECK_VK_ERR << vkAllocateMemory(g_Device, &alloc_info, g_Allocator, &tex_data.image_memory);
		CHECK_VK_ERR << vkBindImageMemory(g_Device, tex_data.image, tex_data.image_memory, 0);
	}

	{
		const VkImageViewCreateInfo info{
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.image = tex_data.image,
			.viewType = VK_IMAGE_VIEW_TYPE_2D,
			.format = VK_FORMAT_R8G8B8A8_UNORM,
			.components = {
				.r = VK_COMPONENT_SWIZZLE_IDENTITY,
				.g = VK_COMPONENT_SWIZZLE_IDENTITY,
				.b = VK_COMPONENT_SWIZZLE_IDENTITY,
				.a = VK_COMPONENT_SWIZZLE_IDENTITY,
			},
			.subresourceRange{
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1,
			},
		};
		CHECK_VK_ERR << vkCreateImageView(g_Device, &info, g_Allocator, &tex_data.image_view);
	}

	{
		const VkSamplerCreateInfo sampler_info{
			.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.magFilter = VK_FILTER_LINEAR,
			.minFilter = VK_FILTER_LINEAR,
			.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
			.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
			.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
			.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
			.mipLodBias = 0,
			.anisotropyEnable = false,
			.maxAnisotropy = 1.0F,
			.compareEnable = false,
			.compareOp = VK_COMPARE_OP_NEVER,
			.minLod = -1000,
			.maxLod = 1000,
			.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK,
			.unnormalizedCoordinates = false,
		};
		CHECK_VK_ERR << vkCreateSampler(g_Device, &sampler_info, g_Allocator, &tex_data.sampler);
	}

	tex_data.descriptor_set = ImGui_ImplVulkan_AddTexture(tex_data.sampler, tex_data.image_view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	{
		const VkBufferCreateInfo buffer_info{
			.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.size = image_size,
			.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
			.queueFamilyIndexCount = 0,
			.pQueueFamilyIndices = nullptr,
		};
		CHECK_VK_ERR << vkCreateBuffer(g_Device, &buffer_info, g_Allocator, &tex_data.upload_buffer);

		VkMemoryRequirements req;
		vkGetBufferMemoryRequirements(g_Device, tex_data.upload_buffer, &req);

		const VkMemoryAllocateInfo alloc_info{
			.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
			.pNext = nullptr,
			.allocationSize = req.size,
			.memoryTypeIndex = find_memory_type(req.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT),
		};
		CHECK_VK_ERR << vkAllocateMemory(g_Device, &alloc_info, g_Allocator, &tex_data.upload_buffer_memory);
		CHECK_VK_ERR << vkBindBufferMemory(g_Device, tex_data.upload_buffer, tex_data.upload_buffer_memory, 0);
	}

	{
		void* map = nullptr;
		CHECK_VK_ERR << vkMapMemory(g_Device, tex_data.upload_buffer_memory, 0, image_size, 0, &map);
		std::memcpy(map, rgba, image_size);
		const VkMappedMemoryRange range{
			.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
			.pNext = nullptr,
			.memory = tex_data.upload_buffer_memory,
			.offset = 0,
			.size = image_size,
		};
		CHECK_VK_ERR << vkFlushMappedMemoryRanges(g_Device, 1, &range);
		vkUnmapMemory(g_Device, tex_data.upload_buffer_memory);
	}

	VkCommandBuffer command_buffer = nullptr;
	{
		const VkCommandBufferAllocateInfo alloc_info{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.pNext = nullptr,
			.commandPool = current_command_pool,
			.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			.commandBufferCount = 1,
		};
		CHECK_VK_ERR << vkAllocateCommandBuffers(g_Device, &alloc_info, &command_buffer);

		const VkCommandBufferBeginInfo begin_info{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.pNext = nullptr,
			.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
			.pInheritanceInfo = nullptr,
		};
		CHECK_VK_ERR << vkBeginCommandBuffer(command_buffer, &begin_info);
	}

	{
		const VkImageMemoryBarrier copy_barrier{
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			.pNext = nullptr,
			.srcAccessMask = 0,
			.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
			.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.image = tex_data.image,
			.subresourceRange = {
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1,
			},
		};
		vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &copy_barrier);

		const VkBufferImageCopy region{
			.bufferOffset = 0,
			.bufferRowLength = 0,
			.bufferImageHeight = 0,
			.imageSubresource = {
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.mipLevel = 0,
				.baseArrayLayer = 0,
				.layerCount = 1,
			},
			.imageOffset = {
				.x = 0,
				.y = 0,
				.z = 0,
			},
			.imageExtent = {
				.width = static_cast<uint32_t>(width),
				.height = static_cast<uint32_t>(height),
				.depth = 1,
			},
		};
		vkCmdCopyBufferToImage(command_buffer, tex_data.upload_buffer, tex_data.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

		const VkImageMemoryBarrier use_barrier{
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			.pNext = nullptr,
			.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
			.dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
			.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.image = tex_data.image,
			.subresourceRange = {
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1,
			},
		};
		vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &use_barrier);
	}

	{
		const VkSubmitInfo end_info{
			.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
			.pNext = nullptr,
			.waitSemaphoreCount = 0,
			.pWaitSemaphores = nullptr,
			.pWaitDstStageMask = nullptr,
			.commandBufferCount = 1,
			.pCommandBuffers = &command_buffer,
			.signalSemaphoreCount = 0,
			.pSignalSemaphores = nullptr,
		};
		CHECK_VK_ERR << vkEndCommandBuffer(command_buffer);
		CHECK_VK_ERR << vkQueueSubmit(current_queue, 1, &end_info, VK_NULL_HANDLE);
		CHECK_VK_ERR << vkDeviceWaitIdle(g_Device);
	}

	return tex_data;
}

void GUI::ImageLoader::destroy_texture(const TextureData& tex_data)
{
	vkFreeMemory(g_Device, tex_data.upload_buffer_memory, g_Allocator);
	vkDestroyBuffer(g_Device, tex_data.upload_buffer, g_Allocator);
	vkDestroySampler(g_Device, tex_data.sampler, g_Allocator);
	vkDestroyImageView(g_Device, tex_data.image_view, g_Allocator);
	vkDestroyImage(g_Device, tex_data.image, g_Allocator);
	vkFreeMemory(g_Device, tex_data.image_memory, g_Allocator);
	ImGui_ImplVulkan_RemoveTexture(tex_data.descriptor_set);
}
