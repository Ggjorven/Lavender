#pragma once

#include <vulkan/vulkan.h>

#ifdef BL_DIST
	#define BL_VALIDATION_LAYERS 0
#else
	#define BL_VALIDATION_LAYERS 1
#endif

static const std::vector<const char*> s_RequestedValidationLayers = { "VK_LAYER_KHRONOS_validation" };
static const std::vector<const char*> s_RequestedDeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

namespace Lavender
{

	class VulkanManager;
	class VulkanHelper;

	class VulkanInstance
	{
	public:
		VulkanInstance();
		virtual ~VulkanInstance();
	
	private:
		void CreateInstance();
		void CreateDebugger();
		void CreateSurface();
		void PickPhysicalDevice();
		void CreateDevice();

	private:
		VkInstance m_Instance = VK_NULL_HANDLE;
		VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;
		VkSurfaceKHR m_Surface = VK_NULL_HANDLE;

		VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
		VkDevice m_Device = VK_NULL_HANDLE;

		VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
		VkQueue m_PresentQueue = VK_NULL_HANDLE;

		friend class VulkanManager;
		friend class VulkanHelper;
	};

}