#pragma once

#include <memory>

#include <vulkan/vulkan.h>

#include "Lavender/APIs/Vulkan/VulkanPhysicalDevice.hpp"

namespace Lavender
{

	class VulkanDevice
	{
	public:
		VulkanDevice(std::shared_ptr<VulkanPhysicalDevice> physicalDevice);
		virtual ~VulkanDevice();

		void Destroy();

		inline VkDevice& GetVulkanDevice() { return m_LogicalDevice; }

		inline VkQueue& GetGraphicsQueue() { return m_GraphicsQueue; }
		inline VkQueue& GetPresentQueue() { return m_PresentQueue; }

		inline std::shared_ptr<VulkanPhysicalDevice> GetPhysicalDevice() const { return m_PhysicalDevice; }

		static std::shared_ptr<VulkanDevice> Create(std::shared_ptr<VulkanPhysicalDevice> physicalDevice);

	private:
		VkDevice m_LogicalDevice = VK_NULL_HANDLE;
		std::shared_ptr<VulkanPhysicalDevice> m_PhysicalDevice = VK_NULL_HANDLE;

		VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
		VkQueue m_PresentQueue = VK_NULL_HANDLE;
	};

}