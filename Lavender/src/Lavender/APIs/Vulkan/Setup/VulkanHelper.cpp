#include "lvpch.h"
#include "VulkanHelper.hpp"

#include <GLFW/glfw3.h>

#include "Lavender/Core/Application.hpp"
#include "Lavender/Core/Logging.hpp"

#include "Lavender/Renderer/Renderer.hpp"

#include "Lavender/APIs/Vulkan/Setup/VulkanInstance.hpp"
#include "Lavender/APIs/Vulkan/VulkanManager.hpp"
#include "Lavender/Renderer/GraphicsContext.hpp"

namespace Lavender
{

	VulkanHelper::QueueFamilyIndices VulkanHelper::QueueFamilyIndices::Find(VulkanInstance* instance, const VkPhysicalDevice& device)
	{
		VulkanHelper::QueueFamilyIndices indices;
		
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
		
		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
		
		int32_t i = 0;
		for (const auto& queueFamily : queueFamilies)
		{
			// Early exit check
			if (indices.IsComplete())
				break;
		
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
				indices.GraphicsFamily = i;
		
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, instance->m_Surface, &presentSupport);
			if (presentSupport)
				indices.PresentFamily = i;
		
			i++;
		}
		
		return indices;
	}

	VulkanHelper::SwapChainSupportDetails VulkanHelper::SwapChainSupportDetails::Query(VulkanInstance* instance, const VkPhysicalDevice& device)
	{
		VulkanHelper::SwapChainSupportDetails details;
		
		// Capabilities
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, instance->m_Surface, &details.Capabilities);
		
		// Formats
		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, instance->m_Surface, &formatCount, nullptr);
		
		if (formatCount != 0)
		{
			details.Formats.resize((size_t)formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, instance->m_Surface, &formatCount, details.Formats.data());
		}
		
		// Presentation modes
		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, instance->m_Surface, &presentModeCount, nullptr);
		
		if (presentModeCount != 0)
		{
			details.PresentModes.resize((size_t)presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, instance->m_Surface, &presentModeCount, details.PresentModes.data());
		}
		
		return details;
	}

	bool VulkanHelper::ValidationLayersSupported()
	{
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		// Check if all requested layers are actually accessible 
		for (const char* layerName : s_RequestedValidationLayers)
		{
			bool layerFound = false;

			for (const auto& layerProperties : availableLayers)
			{
				if (strcmp(layerName, layerProperties.layerName) == 0)
				{
					layerFound = true;
					break;
				}
			}

			if (!layerFound)
				return false;
		}

		return true;
	}

	std::vector<const char*> VulkanHelper::GetRequiredExtensions()
	{
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		#if BL_VALIDATION_LAYERS
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		#endif

		//extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME); // Note(Jorben): This line is needed for MacOS

		return extensions;
	}

	bool VulkanHelper::PhysicalDeviceSuitable(VulkanInstance* instance, const VkPhysicalDevice& device)
	{
		QueueFamilyIndices indices = QueueFamilyIndices::Find(instance, device);

		bool extensionsSupported = ExtensionsSupported(device);
		bool swapChainAdequate = false;

		if (extensionsSupported)
		{
			SwapChainSupportDetails swapChainSupport = SwapChainSupportDetails::Query(instance, device);
			swapChainAdequate = !swapChainSupport.Formats.empty() && !swapChainSupport.PresentModes.empty();
		}

		VkPhysicalDeviceFeatures supportedFeatures;
		vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

		return indices.IsComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
	}

	bool VulkanHelper::ExtensionsSupported(const VkPhysicalDevice& device)
	{
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string> requiredExtensions(s_RequestedDeviceExtensions.begin(), s_RequestedDeviceExtensions.end());

		for (const auto& extension : availableExtensions)
			requiredExtensions.erase(extension.extensionName);

		// Note(Jorben): It's empty if all the required extensions are available
		return requiredExtensions.empty();
	}

	VkSurfaceFormatKHR VulkanHelper::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
	{
		for (const auto& availableFormat : availableFormats)
		{
			if (availableFormat.format == (VkFormat)((int)Renderer::GetAPISpecs().ColourSpace) && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				return availableFormat;
		}

		// Note(Jorben): If nothing 100% satisfies our needs it okay to just go with the first one.
		return availableFormats[0];
	}

	VkPresentModeKHR VulkanHelper::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
	{
		static bool first = true;

		for (const auto& availablePresentMode : availablePresentModes)
		{
			bool vsync = (first ? GraphicsContext::Get()->m_CreationVSync : Application::Get().GetWindow().IsVSync());

			if (!vsync && (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR))
			{
				first = false;
				return availablePresentMode;
			}

			else if (vsync && availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				first = false;
				return availablePresentMode;
			}
		}

		first = false;
		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D VulkanHelper::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
	{
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
			return capabilities.currentExtent;
		else
		{
			auto handle = reinterpret_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());

			int width, height;
			glfwGetFramebufferSize(handle, &width, &height);

			VkExtent2D actualExtent =
			{
				static_cast<uint32_t>(width),
				static_cast<uint32_t>(height)
			};

			actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
			actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

			return actualExtent;
		}
	}

	VkFormat VulkanHelper::FindDepthFormat()
	{
		return FindSupportedFormat({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT }, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
	}

	VkFormat VulkanHelper::FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
	{
		for (VkFormat format : candidates)
		{
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(VulkanManager::GetDeviceInfo().PhysicalDevice, format, &props);

			if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
				return format;
			else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
				return format;
		}

		LV_LOG_ERROR("Failed to find supported format!");
		return VK_FORMAT_UNDEFINED;
	}

}
