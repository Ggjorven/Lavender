#pragma once

#include <optional>
#include <vector>

#include <vulkan/vulkan.h>

namespace Lavender
{

	class VulkanInstance;

	class VulkanHelper
	{
	public:
		struct QueueFamilyIndices
		{
		public:
			std::optional<uint32_t> GraphicsFamily;
			std::optional<uint32_t> PresentFamily;

			static QueueFamilyIndices Find(VulkanInstance* instance, const VkPhysicalDevice& device);

		public:
			inline bool IsComplete() const { return GraphicsFamily.has_value() && PresentFamily.has_value(); }
		};

		struct SwapChainSupportDetails
		{
		public:
			VkSurfaceCapabilitiesKHR Capabilities;
			std::vector<VkSurfaceFormatKHR> Formats;
			std::vector<VkPresentModeKHR> PresentModes;

			static SwapChainSupportDetails Query(VulkanInstance* instance, const VkPhysicalDevice& device);
		};

	public:
		static bool ValidationLayersSupported();
		static std::vector<const char*> GetRequiredExtensions();
		static bool PhysicalDeviceSuitable(VulkanInstance* instance, const VkPhysicalDevice& device);
		static bool ExtensionsSupported(const VkPhysicalDevice& device);

	public:
		static VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		static VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		static VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
		
		static VkFormat FindDepthFormat();
		static VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	};

}