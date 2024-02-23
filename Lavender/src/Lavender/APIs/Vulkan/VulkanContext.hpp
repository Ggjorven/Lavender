#pragma once

#include "Lavender/Renderer/RenderingContext.hpp"

#include "Lavender/APIs/Vulkan/VulkanSwapChain.hpp"

#include <vulkan/vulkan.h>

namespace Lavender
{

	class VulkanContext : public RenderingContext
	{
	public:
		VulkanContext();
		virtual ~VulkanContext();

		void Init() override;
		void Destroy() override;

	private:
		// TODO: Add a lot of vulkan related stuff
		VkInstance m_VulkanInstance = VK_NULL_HANDLE;
		VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;

		//std::shared_ptr<VulkanPhysicalDevice> m_PhysicalDevice = VK_NULL_HANDLE;
		//std::shared_ptr<VulkanDevice> m_Device = VK_NULL_HANDLE;

		//std::shared_ptr<VulkanSwapChain> m_SwapChain = VK_NULL_HANDLE;
	};
}