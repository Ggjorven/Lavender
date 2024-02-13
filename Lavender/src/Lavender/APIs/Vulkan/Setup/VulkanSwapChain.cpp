#include "lvpch.h"
#include "VulkanSwapChain.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/APIs/Vulkan/Setup/VulkanInstance.hpp"
#include "Lavender/APIs/Vulkan/Setup/VulkanHelper.hpp"
#include "Lavender/APIs/Vulkan/Setup/VulkanImageHelper.hpp"
#include "Lavender/APIs/Vulkan/VulkanManager.hpp"

#include "Lavender/Renderer/GraphicsContext.hpp"

namespace Lavender
{

	VulkanSwapChain* VulkanSwapChain::s_Instance = nullptr;

	VulkanSwapChain::VulkanSwapChain()
	{
		s_Instance = this;

		CreateSwapChain();
		CreateImageViews();
	}

	VulkanSwapChain::~VulkanSwapChain()
	{
		CleanUp();

		s_Instance = nullptr;
	}

	void VulkanSwapChain::RecreateSwapChain()
	{
		CleanUp();

		CreateSwapChain();
		CreateImageViews();

		VulkanManager::PopulateSwapChainInfo();
	}

	void VulkanSwapChain::CreateSwapChain()
	{
		VulkanDeviceInfo& info = VulkanManager::GetDeviceInfo();

		VulkanHelper::SwapChainSupportDetails swapChainSupport = VulkanHelper::SwapChainSupportDetails::Query(GraphicsContext::Get()->GetInstance(), info.PhysicalDevice);

		VkSurfaceFormatKHR surfaceFormat = VulkanHelper::ChooseSwapSurfaceFormat(swapChainSupport.Formats);
		VkPresentModeKHR presentMode = VulkanHelper::ChooseSwapPresentMode(swapChainSupport.PresentModes);
		VkExtent2D extent = VulkanHelper::ChooseSwapExtent(swapChainSupport.Capabilities);

		// Note(Jorben): +1 because sticking to the minimum can cause us to wait on the driver sometimes
		uint32_t imageCount = swapChainSupport.Capabilities.minImageCount + 1;

		// Making sure we don't exceed the maximum
		if (swapChainSupport.Capabilities.maxImageCount > 0 && imageCount > swapChainSupport.Capabilities.maxImageCount)
			imageCount = swapChainSupport.Capabilities.maxImageCount;

		VkSwapchainCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = info.Surface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		VulkanHelper::QueueFamilyIndices indices = VulkanHelper::QueueFamilyIndices::Find(GraphicsContext::Get()->GetInstance(), info.PhysicalDevice);
		uint32_t queueFamilyIndices[] = { indices.GraphicsFamily.value(), indices.PresentFamily.value() };

		if (indices.GraphicsFamily != indices.PresentFamily)
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0; // Optional
			createInfo.pQueueFamilyIndices = nullptr; // Optional
		}

		createInfo.preTransform = swapChainSupport.Capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;

		createInfo.oldSwapchain = VK_NULL_HANDLE;

		// Creation of the swapchain
		if (vkCreateSwapchainKHR(info.Device, &createInfo, nullptr, &m_SwapChain) != VK_SUCCESS)
			LV_LOG_ERROR("Failed to create swap chain!");

		// Note(Jorben): We query the amount of images again, because vulkan is allowed to create a swapchain with more images.
		vkGetSwapchainImagesKHR(info.Device, m_SwapChain, &imageCount, nullptr);
		m_SwapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(info.Device, m_SwapChain, &imageCount, m_SwapChainImages.data());

		// Store the format and extent for the future
		m_SwapChainImageFormat = surfaceFormat.format;
		m_SwapChainExtent = extent;
	}

	void VulkanSwapChain::CreateImageViews()
	{
		m_SwapChainImageViews.resize(m_SwapChainImages.size());
		
		for (size_t i = 0; i < m_SwapChainImages.size(); i++)
			m_SwapChainImageViews[i] = VulkanImageHelper::CreateImageView(m_SwapChainImages[i], m_SwapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
	}

	void VulkanSwapChain::CleanUp()
	{
		VulkanDeviceInfo& info = VulkanManager::GetDeviceInfo();

		for (size_t i = 0; i < m_SwapChainImageViews.size(); i++)
			vkDestroyImageView(info.Device, m_SwapChainImageViews[i], nullptr);

		vkDestroySwapchainKHR(info.Device, m_SwapChain, nullptr);
	}

}