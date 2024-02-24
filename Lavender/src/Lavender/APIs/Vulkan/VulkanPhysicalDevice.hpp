#pragma once

#include <stdint.h>
#include <memory>

#include <vulkan/vulkan.h>

namespace Lavender
{

	struct QueueFamilyIndices
	{
	public:
		std::optional<uint32_t> GraphicsFamily;
		std::optional<uint32_t> PresentFamily;

		static QueueFamilyIndices Find(const VkPhysicalDevice& device);

	public:
		inline bool IsComplete() const { return GraphicsFamily.has_value() && PresentFamily.has_value(); }
	};

	struct SwapChainSupportDetails
	{
	public:
		VkSurfaceCapabilitiesKHR Capabilities;
		std::vector<VkSurfaceFormatKHR> Formats;
		std::vector<VkPresentModeKHR> PresentModes;

		static SwapChainSupportDetails Query(const VkPhysicalDevice& device);
	};

	class VulkanPhysicalDevice
	{
	public:
		VulkanPhysicalDevice();
		virtual ~VulkanPhysicalDevice();

		inline VkPhysicalDevice& GetVulkanPhysicalDevice() { return m_PhysicalDevice; }

		static std::shared_ptr<VulkanPhysicalDevice> Select();

	private:
		bool PhysicalDeviceSuitable(const VkPhysicalDevice& device);
		bool ExtensionsSupported(const VkPhysicalDevice& device);

	private:
		VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
	};

}