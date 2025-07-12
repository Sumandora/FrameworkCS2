#pragma once

#include "backends/imgui_impl_vulkan.h"

#include <vulkan/vulkan.h>

#include <vector>

#include "../../Utils/Logging.hpp"

static constexpr uint32_t g_MinImageCount = 2;

inline VkAllocationCallbacks* g_Allocator = nullptr;
inline VkInstance g_Instance = VK_NULL_HANDLE;
inline VkPhysicalDevice g_PhysicalDevice = VK_NULL_HANDLE;
inline VkDevice g_FakeDevice = VK_NULL_HANDLE, g_Device = VK_NULL_HANDLE;
inline uint32_t g_QueueFamily = (uint32_t)-1;
inline std::vector<VkQueueFamilyProperties> g_QueueFamilies;
inline VkPipelineCache g_PipelineCache = VK_NULL_HANDLE;
inline VkDescriptorPool g_DescriptorPool = VK_NULL_HANDLE;
inline VkRenderPass g_RenderPass = VK_NULL_HANDLE;
inline ImGui_ImplVulkanH_Frame g_Frames[8] = {};

inline VkQueue current_queue = VK_NULL_HANDLE;
inline VkCommandPool current_command_pool = VK_NULL_HANDLE;

struct VulkanErrorSink {
	constexpr VulkanErrorSink() = default;

	const VulkanErrorSink& operator<<(int error_value) const
	{
		if (!error_value)
			return *this;

		Logging::error("vulkan error: {}", error_value);

		return *this;
	}
};

static constexpr VulkanErrorSink CHECK_VK_ERR{};
