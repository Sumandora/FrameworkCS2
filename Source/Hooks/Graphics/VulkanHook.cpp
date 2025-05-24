// #############################################
// # CREDITS: https://github.com/bruhmoment21/ #
// #############################################

#include "GraphicsHook.hpp"

#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_vulkan.h"
#include "imgui.h"
#include "imgui_internal.h"

#include <cstdint>
#include <memory>
#include <SDL3/SDL_video.h>
#include <vector>
#include <vulkan/vulkan.h>

#include "DetourHooking.hpp"

#include "BCRL/Session.hpp"

#include "../../Memory.hpp"

#include "../../GUI/GUI.hpp"

#include "../../Utils/Logging.hpp"
#include "../../Utils/UninitializedObject.hpp"

static constexpr uint32_t g_MinImageCount = 2;

static VkAllocationCallbacks* g_Allocator = nullptr;
static VkInstance g_Instance = VK_NULL_HANDLE;
static VkPhysicalDevice g_PhysicalDevice = VK_NULL_HANDLE;
static VkDevice g_FakeDevice = VK_NULL_HANDLE, g_Device = VK_NULL_HANDLE;
static uint32_t g_QueueFamily = (uint32_t)-1;
static std::vector<VkQueueFamilyProperties> g_QueueFamilies;
static VkPipelineCache g_PipelineCache = VK_NULL_HANDLE;
static VkDescriptorPool g_DescriptorPool = VK_NULL_HANDLE;
static VkRenderPass g_RenderPass = VK_NULL_HANDLE;
static ImGui_ImplVulkanH_Frame g_Frames[8] = {};

static VkQueue GetGraphicQueue()
{
	for (uint32_t i = 0; i < g_QueueFamilies.size(); ++i) {
		const VkQueueFamilyProperties& family = g_QueueFamilies[i];
		for (uint32_t j = 0; j < family.queueCount; ++j) {
			VkQueue it = VK_NULL_HANDLE;
			vkGetDeviceQueue(g_Device, i, j, &it);

			if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				return it;
			}
		}
	}

	return VK_NULL_HANDLE;
}

bool is_using_wayland()
{
	static const char* driver = []() {
		const char* driver = SDL_GetCurrentVideoDriver();
		Logging::info("SDL Video Driver: {}", driver);
		return driver;
	}();

	return strcmp(driver, "wayland") == 0;
}

static void CreateDevice()
{
	// Create Vulkan Instance
	{
		VkInstanceCreateInfo create_info = {};
		constexpr const char* instance_extension = "VK_KHR_surface";

		create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		create_info.enabledExtensionCount = 1;
		create_info.ppEnabledExtensionNames = &instance_extension;

		// Create Vulkan Instance without any debug feature
		vkCreateInstance(&create_info, g_Allocator, &g_Instance);
	}

	// Select GPU
	{
		uint32_t gpu_count;
		vkEnumeratePhysicalDevices(g_Instance, &gpu_count, nullptr);
		IM_ASSERT(gpu_count > 0);

		auto* gpus = new VkPhysicalDevice[sizeof(VkPhysicalDevice) * gpu_count];
		vkEnumeratePhysicalDevices(g_Instance, &gpu_count, gpus);

		// If a number >1 of GPUs got reported, find discrete GPU if present, or
		// use first one available. This covers most common cases
		// (multi-gpu/integrated+dedicated graphics). Handling more complicated
		// setups (multiple dedicated GPUs) is out of scope of this sample.
		int use_gpu = 0;
		for (int i = 0; i < (int)gpu_count; ++i) {
			VkPhysicalDeviceProperties properties;
			vkGetPhysicalDeviceProperties(gpus[i], &properties);
			if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
				use_gpu = i;
				break;
			}
		}

		g_PhysicalDevice = gpus[use_gpu];
		delete[] gpus;
	}

	// Select graphics queue family
	{
		uint32_t count;
		vkGetPhysicalDeviceQueueFamilyProperties(g_PhysicalDevice, &count, nullptr);
		g_QueueFamilies.resize(count);
		vkGetPhysicalDeviceQueueFamilyProperties(g_PhysicalDevice, &count, g_QueueFamilies.data());
		for (uint32_t i = 0; i < count; ++i) {
			if (g_QueueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				g_QueueFamily = i;
				break;
			}
		}
		IM_ASSERT(g_QueueFamily != (uint32_t)-1);
	}

	// Create Logical Device (with 1 queue)
	{
		constexpr const char* device_extension = "VK_KHR_swapchain";
		constexpr const float queue_priority = 1.0f;

		VkDeviceQueueCreateInfo queue_info = {};
		queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queue_info.queueFamilyIndex = g_QueueFamily;
		queue_info.queueCount = 1;
		queue_info.pQueuePriorities = &queue_priority;

		VkDeviceCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		create_info.queueCreateInfoCount = 1;
		create_info.pQueueCreateInfos = &queue_info;
		create_info.enabledExtensionCount = 1;
		create_info.ppEnabledExtensionNames = &device_extension;

		vkCreateDevice(g_PhysicalDevice, &create_info, g_Allocator, &g_FakeDevice);
	}
}

static void CreateRenderTarget(VkDevice device, VkSwapchainKHR swapchain)
{
	uint32_t uImageCount;
	vkGetSwapchainImagesKHR(device, swapchain, &uImageCount, nullptr);

	VkImage backbuffers[8]{};
	vkGetSwapchainImagesKHR(device, swapchain, &uImageCount, backbuffers);

	for (uint32_t i = 0; i < uImageCount; ++i) {
		g_Frames[i].Backbuffer = backbuffers[i];

		ImGui_ImplVulkanH_Frame* fd = &g_Frames[i];
		{
			VkCommandPoolCreateInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			info.queueFamilyIndex = g_QueueFamily;

			vkCreateCommandPool(device, &info, g_Allocator, &fd->CommandPool);
		}
		{
			VkCommandBufferAllocateInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			info.commandPool = fd->CommandPool;
			info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			info.commandBufferCount = 1;

			vkAllocateCommandBuffers(device, &info, &fd->CommandBuffer);
		}
	}

	// Create the Render Pass
	{
		VkAttachmentDescription attachment = {};
		attachment.format = is_using_wayland() ? VK_FORMAT_R8G8B8A8_UNORM : VK_FORMAT_B8G8R8A8_UNORM;
		attachment.samples = VK_SAMPLE_COUNT_1_BIT;
		attachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference color_attachment = {};
		color_attachment.attachment = 0;
		color_attachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &color_attachment;

		VkRenderPassCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		info.attachmentCount = 1;
		info.pAttachments = &attachment;
		info.subpassCount = 1;
		info.pSubpasses = &subpass;

		vkCreateRenderPass(device, &info, g_Allocator, &g_RenderPass);
	}

	// Create The Image Views
	{
		VkImageViewCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		info.format = is_using_wayland() ? VK_FORMAT_R8G8B8A8_UNORM : VK_FORMAT_B8G8R8A8_UNORM;

		info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		info.subresourceRange.baseMipLevel = 0;
		info.subresourceRange.levelCount = 1;
		info.subresourceRange.baseArrayLayer = 0;
		info.subresourceRange.layerCount = 1;

		for (uint32_t i = 0; i < uImageCount; ++i) {
			ImGui_ImplVulkanH_Frame* fd = &g_Frames[i];
			info.image = fd->Backbuffer;

			vkCreateImageView(device, &info, g_Allocator, &fd->BackbufferView);
		}
	}

	// Create Framebuffer
	{
		VkImageView attachment[1];
		VkFramebufferCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		info.renderPass = g_RenderPass;
		info.attachmentCount = 1;
		info.pAttachments = attachment;
		info.layers = 1;

		for (uint32_t i = 0; i < uImageCount; ++i) {
			ImGui_ImplVulkanH_Frame* fd = &g_Frames[i];
			attachment[0] = fd->BackbufferView;

			vkCreateFramebuffer(device, &info, g_Allocator, &fd->Framebuffer);
		}
	}

	if (!g_DescriptorPool) // Create Descriptor Pool.
	{
		constexpr VkDescriptorPoolSize pool_sizes[] = { { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 } };

		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
		pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
		pool_info.pPoolSizes = pool_sizes;

		vkCreateDescriptorPool(device, &pool_info, g_Allocator, &g_DescriptorPool);
	}
}

static void CleanupRenderTarget()
{
	for (auto& g_Frame : g_Frames) {
		if (g_Frame.CommandBuffer) {
			vkFreeCommandBuffers(g_Device, g_Frame.CommandPool, 1, &g_Frame.CommandBuffer);
			g_Frame.CommandBuffer = VK_NULL_HANDLE;
		}
		if (g_Frame.CommandPool) {
			vkDestroyCommandPool(g_Device, g_Frame.CommandPool, g_Allocator);
			g_Frame.CommandPool = VK_NULL_HANDLE;
		}
		if (g_Frame.BackbufferView) {
			vkDestroyImageView(g_Device, g_Frame.BackbufferView, g_Allocator);
			g_Frame.BackbufferView = VK_NULL_HANDLE;
		}
		if (g_Frame.Framebuffer) {
			vkDestroyFramebuffer(g_Device, g_Frame.Framebuffer, g_Allocator);
			g_Frame.Framebuffer = VK_NULL_HANDLE;
		}
	}
}

static void CleanupDevice()
{
	CleanupRenderTarget();

	if (g_DescriptorPool) {
		vkDestroyDescriptorPool(g_Device, g_DescriptorPool, g_Allocator);
		g_DescriptorPool = nullptr;
	}
	if (g_Instance) {
		vkDestroyInstance(g_Instance, g_Allocator);
		g_Instance = nullptr;
	}

	g_Device = nullptr;
}

static void RenderImGui([[maybe_unused]] VkQueue queue, const VkPresentInfoKHR* pPresentInfo)
{
	if (!ImGui::GetCurrentContext() || !g_Device)
		return;
	if (!ImGui::GetIO().BackendPlatformUserData)
		return;

	VkQueue graphicQueue = GetGraphicQueue();
	if (!graphicQueue) {
		Logging::error("No queue that has VK_QUEUE_GRAPHICS_BIT has been found!");
		return;
	}

	for (uint32_t i = 0; i < pPresentInfo->swapchainCount; ++i) {
		VkSwapchainKHR swapchain = pPresentInfo->pSwapchains[i];
		if (g_Frames[0].Framebuffer == VK_NULL_HANDLE) {
			CreateRenderTarget(g_Device, swapchain);
		}

		ImGui_ImplVulkanH_Frame* fd = &g_Frames[pPresentInfo->pImageIndices[i]];
		{
			vkResetCommandBuffer(fd->CommandBuffer, 0);

			VkCommandBufferBeginInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

			vkBeginCommandBuffer(fd->CommandBuffer, &info);
		}
		{
			auto [width, height] = ImGui::GetIO().DisplaySize;

			VkRenderPassBeginInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			info.renderPass = g_RenderPass;
			info.framebuffer = fd->Framebuffer;
			info.renderArea.extent.width = static_cast<uint32_t>(width);
			info.renderArea.extent.height = static_cast<uint32_t>(height);

			vkCmdBeginRenderPass(fd->CommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
		}

		if (!ImGui::GetIO().BackendRendererUserData) {
			ImGui_ImplVulkan_InitInfo init_info = {};
			init_info.Instance = g_Instance;
			init_info.PhysicalDevice = g_PhysicalDevice;
			init_info.Device = g_Device;
			init_info.QueueFamily = g_QueueFamily;
			init_info.Queue = graphicQueue;
			init_info.PipelineCache = g_PipelineCache;
			init_info.DescriptorPool = g_DescriptorPool;
			init_info.Subpass = 0;
			init_info.MinImageCount = g_MinImageCount;
			init_info.ImageCount = g_MinImageCount;
			init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
			init_info.Allocator = g_Allocator;
			init_info.RenderPass = g_RenderPass;

			ImGui_ImplVulkan_Init(&init_info);
		}

		GUI::flush_events();

		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplSDL3_NewFrame();

		GUI::render();

		auto* drawData = ImGui::GetDrawData();
		{
			const std::lock_guard<std::mutex> lock(GraphicsHook::espMutex);
			if (GraphicsHook::espDrawList != nullptr) {
				drawData->AddDrawList(GraphicsHook::espDrawList);
				ImGui_ImplVulkan_RenderDrawData(drawData, fd->CommandBuffer);
			}
		}
		// Record dear imgui primitives into command buffer

		static std::once_flag espDrawListFlag;
		std::call_once(espDrawListFlag, [&]() {
			GraphicsHook::espDrawListSharedData = new ImDrawListSharedData(*ImGui::GetDrawListSharedData());
			GraphicsHook::espDrawList = new ImDrawList(GraphicsHook::espDrawListSharedData);
		});

		// Submit command buffer
		vkCmdEndRenderPass(fd->CommandBuffer);
		vkEndCommandBuffer(fd->CommandBuffer);

		constexpr VkPipelineStageFlags stages_wait = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
		{
			VkSubmitInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

			info.pWaitDstStageMask = &stages_wait;

			info.commandBufferCount = 1;
			info.pCommandBuffers = &fd->CommandBuffer;

			vkQueueSubmit(graphicQueue, 1, &info, VK_NULL_HANDLE);
		}
	}
}

using Hook = DetourHooking::Hook<true, decltype(Memory::mem_mgr)>;

using AcquireNextImageKHRFunc = VkResult (*)(VkDevice, VkSwapchainKHR, uint64_t, VkSemaphore, VkFence, uint32_t*);
static UninitializedObject<Hook> acquireNextImageKHRHook;

static VkResult VKAPI_CALL hkAcquireNextImageKHR(VkDevice device, VkSwapchainKHR swapchain, uint64_t timeout, VkSemaphore semaphore, VkFence fence, uint32_t* pImageIndex)
{
	g_Device = device;

	return reinterpret_cast<AcquireNextImageKHRFunc>(acquireNextImageKHRHook->get_trampoline())(device, swapchain, timeout, semaphore, fence, pImageIndex);
}

using QueuePresentKHRFunc = VkResult (*)(VkQueue, const VkPresentInfoKHR*);
static UninitializedObject<Hook> queuePresentKHRHook;

static VkResult VKAPI_CALL hkQueuePresentKHR(VkQueue queue, const VkPresentInfoKHR* pPresentInfo)
{
	RenderImGui(queue, pPresentInfo);

	return reinterpret_cast<QueuePresentKHRFunc>(queuePresentKHRHook->get_trampoline())(queue, pPresentInfo);
}

using CreateSwapchainKHRFunc = VkResult (*)(VkDevice, const VkSwapchainCreateInfoKHR*, const VkAllocationCallbacks*, VkSwapchainKHR*);
static UninitializedObject<Hook> createSwapchainKHRHook;

static VkResult VKAPI_CALL hkCreateSwapchainKHR(VkDevice device, const VkSwapchainCreateInfoKHR* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSwapchainKHR* pSwapchain)
{
	CleanupRenderTarget();

	return reinterpret_cast<CreateSwapchainKHRFunc>(createSwapchainKHRHook->get_trampoline())(device, pCreateInfo, pAllocator, pSwapchain);
}

bool GraphicsHook::hookVulkan()
{
	CreateDevice();

	if (!g_FakeDevice) {
		Logging::error("Couldn't create device");
		return false;
	}

	auto acquireNextImageKHR = vkGetDeviceProcAddr(g_FakeDevice, "vkAcquireNextImageKHR");
	auto queuePresentKHR = vkGetDeviceProcAddr(g_FakeDevice, "vkQueuePresentKHR");
	auto createSwapchainKHR = vkGetDeviceProcAddr(g_FakeDevice, "vkCreateSwapchainKHR");

	vkDestroyDevice(g_FakeDevice, g_Allocator);

	auto hookLength = [](auto* start) {
		std::uintptr_t cPointer = reinterpret_cast<std::uintptr_t>(start);
		auto ptr = BCRL::pointer(Memory::mem_mgr, cPointer).repeater([cPointer](auto& safePointer) {
															   safePointer = safePointer.next_instruction();
															   return safePointer.get_pointer() - cPointer < DetourHooking::MIN_LENGTH;
														   })
					   .finalize();

		return ptr.value() - cPointer;
	};

	acquireNextImageKHRHook.emplace(Memory::emalloc, reinterpret_cast<void*>(acquireNextImageKHR), reinterpret_cast<void*>(hkAcquireNextImageKHR), hookLength(acquireNextImageKHR));
	queuePresentKHRHook.emplace(Memory::emalloc, reinterpret_cast<void*>(queuePresentKHR), reinterpret_cast<void*>(hkQueuePresentKHR), hookLength(queuePresentKHR));
	createSwapchainKHRHook.emplace(Memory::emalloc, reinterpret_cast<void*>(createSwapchainKHR), reinterpret_cast<void*>(hkCreateSwapchainKHR), hookLength(createSwapchainKHR));

	acquireNextImageKHRHook->enable();
	queuePresentKHRHook->enable();
	createSwapchainKHRHook->enable();

	return true;
}

void GraphicsHook::unhookVulkan()
{
	// Destroy hooks
	acquireNextImageKHRHook.reset();
	queuePresentKHRHook.reset();
	createSwapchainKHRHook.reset();

	const ImGuiIO& io = ImGui::GetIO();

	if (io.BackendRendererUserData) {
		ImGui_ImplVulkan_Shutdown();
	}

	CleanupDevice();
}
