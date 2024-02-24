#pragma once

#include <memory>

#include <vulkan/vulkan.h>

#include "Lavender/Renderer/RenderingContext.hpp"

#include "Lavender/APIs/Vulkan/VulkanPhysicalDevice.hpp"
#include "Lavender/APIs/Vulkan/VulkanDevice.hpp"
#include "Lavender/APIs/Vulkan/VulkanSwapChain.hpp"

namespace Lavender
{

	#ifndef LV_DIST
		inline static constexpr const bool s_Validation = true;
	#else
		inline static constexpr const bool s_Validation = false;
	#endif

	static const std::vector<const char*> s_RequestedValidationLayers = { "VK_LAYER_KHRONOS_validation" };
	static const std::vector<const char*> s_RequestedDeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	class VulkanContext : public RenderingContext
	{
	public:
		VulkanContext();
		virtual ~VulkanContext();

		void Init() override;
		void Destroy() override;

		inline VkInstance& GetVulkanInstance() { return m_VulkanInstance; }
		inline VkSurfaceKHR& GetVulkanSurface() { return m_Surface; }

		inline std::shared_ptr<VulkanDevice> GetLogicalDevice() { return m_Device; }
		inline std::shared_ptr<VulkanPhysicalDevice> GetPhysicalDevice() { return m_PhysicalDevice; }

	private:
		VkInstance m_VulkanInstance = VK_NULL_HANDLE;
		VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;
		VkSurfaceKHR m_Surface = VK_NULL_HANDLE;

		std::shared_ptr<VulkanPhysicalDevice> m_PhysicalDevice = VK_NULL_HANDLE;
		std::shared_ptr<VulkanDevice> m_Device = VK_NULL_HANDLE;
		std::shared_ptr<VulkanSwapChain> m_SwapChain = VK_NULL_HANDLE;
	};

}