#pragma once

#include <memory>
#include <vector>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include "tracy/Tracy.hpp"
#include "tracy/TracyVulkan.hpp"

namespace Lavender
{

	class VulkanInstance;
	class VulkanSwapChain;
	class VulkanResources;

	struct VulkanDeviceInfo
	{
	public:
		VkInstance Instance = VK_NULL_HANDLE;
		VkDebugUtilsMessengerEXT DebugMessenger = VK_NULL_HANDLE;
		VkSurfaceKHR Surface = VK_NULL_HANDLE;

		VkPhysicalDevice PhysicalDevice = VK_NULL_HANDLE;
		VkDevice Device = VK_NULL_HANDLE;

		VkQueue GraphicsQueue = VK_NULL_HANDLE;
		VkQueue PresentQueue = VK_NULL_HANDLE;
	};

	struct VulkanSwapChainInfo
	{
	public:
		VkSwapchainKHR SwapChain = VK_NULL_HANDLE;
		VkFormat SwapChainImageFormat = VkFormat::VK_FORMAT_UNDEFINED;
		VkExtent2D SwapChainExtent = { };

		std::vector<VkImage> SwapChainImages = { };
		std::vector<VkImageView> SwapChainImageViews = { };
	};

	struct VulkanResourceInfo
	{
	public:
		VkCommandPool CommandPool = VK_NULL_HANDLE;
		std::vector<VkCommandBuffer> CommandBuffers = { };

		std::vector<VkSemaphore> ImageAvailableSemaphores = { };
		std::vector<VkSemaphore> RenderFinishedSemaphores = { };
		std::vector<VkFence> InFlightFences = { };

		VkImage DepthImage = VK_NULL_HANDLE;
		VmaAllocation DepthImageMemory = VK_NULL_HANDLE;
		VkImageView DepthImageView = VK_NULL_HANDLE;
	};



	class VulkanManager
	{
	public:
		VulkanManager() = default;
		virtual ~VulkanManager() = default;

		static void Init();

		static void RecreateSwapChain();

		static VulkanDeviceInfo& GetDeviceInfo() { return s_Instance->m_DeviceInfo; }
		static VulkanSwapChainInfo& GetSwapChainInfo() { return s_Instance->m_SwapChainInfo; }
		static VulkanResourceInfo& GetResourceInfo() { return s_Instance->m_ResourceInfo; }

		static void PopulateDeviceInfo();
		static void PopulateSwapChainInfo();
		static void PopulateResourceInfo();

		static void PopulateDeviceInfo(VulkanInstance* obj);
		static void PopulateSwapChainInfo(VulkanSwapChain* obj);
		static void PopulateResourceInfo(VulkanResources* obj);

	private:
		VulkanDeviceInfo m_DeviceInfo = {};
		VulkanSwapChainInfo m_SwapChainInfo = {};
		VulkanResourceInfo m_ResourceInfo = {};

	private:
		static std::unique_ptr<VulkanManager> s_Instance;
	};

}