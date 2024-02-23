#pragma once

#include "Lavender/Renderer/RenderingContext.hpp"

#include "Lavender/APIs/Vulkan/VulkanSwapChain.hpp"

namespace Lavender
{

	class VulkanContext : public RenderingContext
	{
	public:
		VulkanContext();
		virtual ~VulkanContext();

		void Init() override;

	private:
		// TODO: Add a lot of vulkan related stuff
		// Devices
		//Ref<VulkanPhysicalDevice> m_PhysicalDevice;
		//Ref<VulkanDevice> m_Device;
		//
		//// Vulkan instance
		//inline static VkInstance s_VulkanInstance;
		//	VkDebugUtilsMessengerEXT m_DebugUtilsMessenger = VK_NULL_HANDLE;
		//VkPipelineCache m_PipelineCache = nullptr;
		//
		//VulkanSwapChain m_SwapChain;
	};
}