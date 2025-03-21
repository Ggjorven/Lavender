#pragma once

#include <memory>

#include <vulkan/vulkan.h>

#include "Lavender/APIs/Vulkan/VulkanPhysicalDevice.hpp"
#include "Lavender/Utils/Utils.hpp"

namespace Lavender
{

	class VulkanDevice
	{
	public:
		VulkanDevice(Ref<VulkanPhysicalDevice> physicalDevice);
		virtual ~VulkanDevice();

		void Destroy();

		inline VkDevice& GetVulkanDevice() { return m_LogicalDevice; }

		inline VkQueue& GetGraphicsQueue() { return m_GraphicsQueue; }
		inline VkQueue& GetPresentQueue() { return m_PresentQueue; }

		inline Ref<VulkanPhysicalDevice> GetPhysicalDevice() const { return m_PhysicalDevice; }

		static Ref<VulkanDevice> Create(Ref<VulkanPhysicalDevice> physicalDevice);

	private:
		VkDevice m_LogicalDevice = VK_NULL_HANDLE;
		Ref<VulkanPhysicalDevice> m_PhysicalDevice = VK_NULL_HANDLE;

		VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
		VkQueue m_PresentQueue = VK_NULL_HANDLE;
	};

}