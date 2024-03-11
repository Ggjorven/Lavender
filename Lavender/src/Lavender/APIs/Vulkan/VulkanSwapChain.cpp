#include "lvpch.h"
#include "VulkanSwapChain.hpp"

#include "Lavender/Core/Logging.hpp"
#include "Lavender/Core/Application.hpp"

#include "Lavender/Utils/Utils.hpp"
#include "Lavender/Utils/Profiler.hpp"

#include "Lavender/Renderer/Renderer.hpp"
#include "Lavender/APIs/Vulkan/VulkanRenderer.hpp"
#include "Lavender/APIs/Vulkan/VulkanContext.hpp"
#include "Lavender/APIs/Vulkan/VulkanRenderCommandBuffer.hpp"
#include "Lavender/APIs/Vulkan/VulkanAllocator.hpp"

namespace Lavender
{

	VulkanSwapChain::VulkanSwapChain(VkInstance vkInstance, Ref<VulkanDevice> vkDevice)
		: m_Instance(vkInstance), m_Device(vkDevice)
	{
		FindImageFormatAndColorSpace();
	}

	void VulkanSwapChain::Init(uint32_t width, uint32_t height, const bool vsync)
	{
		VkDevice device = m_Device->GetVulkanDevice();
		VkPhysicalDevice physicalDevice = m_Device->GetPhysicalDevice()->GetVulkanPhysicalDevice();
		VkSurfaceKHR& surface = RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetVulkanSurface();

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Command pools
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		uint32_t framesInFlight = Renderer::GetSpecification().FramesInFlight;
		if (!m_CommandPool)
		{
			QueueFamilyIndices queueFamilyIndices = QueueFamilyIndices::Find(m_Device->GetPhysicalDevice()->GetVulkanPhysicalDevice());

			VkCommandPoolCreateInfo poolInfo = {};
			poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			poolInfo.queueFamilyIndex = queueFamilyIndices.GraphicsFamily.value();

			if (vkCreateCommandPool(m_Device->GetVulkanDevice(), &poolInfo, nullptr, &m_CommandPool) != VK_SUCCESS)
				LV_LOG_ERROR("Failed to create command pool!");
		}

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// SwapChain // TODO: Clean up this section
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		VkSwapchainKHR oldSwapchain = m_SwapChain;

		// Get physical device surface properties and formats
		VkSurfaceCapabilitiesKHR surfCaps;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfCaps);

		// Get available present modes
		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, NULL);
		std::vector<VkPresentModeKHR> presentModes(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes.data());

		VkExtent2D swapchainExtent = {};
		// If width (and height) equals the special value 0xFFFFFFFF, the size of the surface will be set by the swapchain
		if (surfCaps.currentExtent.width == (uint32_t)-1)
		{
			// If the surface size is undefined, the size is set to
			// the size of the images requested.
			swapchainExtent.width = width;
			swapchainExtent.height = height;
		}
		else
		{
			// If the surface size is defined, the swap chain size must match
			swapchainExtent = surfCaps.currentExtent;
		}

		if (width == 0 || height == 0)
			return;

		// The VK_PRESENT_MODE_FIFO_KHR mode must always be present as per spec
		// This mode waits for the vertical blank ("v-sync")
		VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;

		// If v-sync is not requested, try to find a mailbox mode
		// It's the lowest latency non-tearing present mode available
		if (!vsync)
		{
			for (size_t i = 0; i < presentModeCount; i++)
			{
				if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
				{
					swapchainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
					break;
				}
				if ((swapchainPresentMode != VK_PRESENT_MODE_MAILBOX_KHR) && (presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR))
				{
					swapchainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
				}
			}
		}

		// Determine the number of images
		uint32_t desiredNumberOfSwapchainImages = surfCaps.minImageCount + 1;
		if ((surfCaps.maxImageCount > 0) && (desiredNumberOfSwapchainImages > surfCaps.maxImageCount))
		{
			desiredNumberOfSwapchainImages = surfCaps.maxImageCount;
		}

		// Find the transformation of the surface
		VkSurfaceTransformFlagsKHR preTransform;
		if (surfCaps.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
		{
			// We prefer a non-rotated transform
			preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		}
		else
		{
			preTransform = surfCaps.currentTransform;
		}

		// Find a supported composite alpha format (not all devices support alpha opaque)
		VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		// Simply select the first composite alpha format available
		std::vector<VkCompositeAlphaFlagBitsKHR> compositeAlphaFlags = {

			VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
			VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
			VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
		};
		for (auto& compositeAlphaFlag : compositeAlphaFlags) 
		{
			if (surfCaps.supportedCompositeAlpha & compositeAlphaFlag) 
			{
				compositeAlpha = compositeAlphaFlag;
				break;
			};
		}

		VkSwapchainCreateInfoKHR swapchainCI = {};
		swapchainCI.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapchainCI.pNext = NULL;
		swapchainCI.surface = surface;
		swapchainCI.minImageCount = desiredNumberOfSwapchainImages;
		swapchainCI.imageFormat = m_ColourFormat;
		swapchainCI.imageColorSpace = m_ColourSpace;
		swapchainCI.imageExtent = { swapchainExtent.width, swapchainExtent.height };

		swapchainCI.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		if (surfCaps.supportedTransforms & VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
			swapchainCI.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		if (surfCaps.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT)
			swapchainCI.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;

		swapchainCI.preTransform = (VkSurfaceTransformFlagBitsKHR)preTransform;
		swapchainCI.imageArrayLayers = 1;
		swapchainCI.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapchainCI.queueFamilyIndexCount = 0;
		swapchainCI.pQueueFamilyIndices = NULL;
		swapchainCI.presentMode = swapchainPresentMode;
		swapchainCI.oldSwapchain = oldSwapchain;
		// Setting clipped to VK_TRUE allows the implementation to discard rendering outside of the surface area
		swapchainCI.clipped = VK_TRUE;
		swapchainCI.compositeAlpha = compositeAlpha;

		// Enable transfer source on swap chain images if supported
		if (surfCaps.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) 
		{
			swapchainCI.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		}

		// Enable transfer destination on swap chain images if supported
		if (surfCaps.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) 
		{
			swapchainCI.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		}

		vkCreateSwapchainKHR(device, &swapchainCI, nullptr, &m_SwapChain);

		if (oldSwapchain)
			vkDestroySwapchainKHR(device, oldSwapchain, nullptr); // Destroys Images?

		for (auto& image : m_Images)
			vkDestroyImageView(device, image.ImageView, nullptr);
		m_Images.clear();

		if (m_DepthStencil.Image)
			VulkanAllocator::DestroyImage(m_DepthStencil.Image, m_DepthStencil.MemoryAlloc);
		if (m_DepthStencil.ImageView)
			vkDestroyImageView(device, m_DepthStencil.ImageView, nullptr);

		// Get the swap chain images
		static uint32_t imageCount = 0;
		static std::vector<VkImage> imageCopies = { };

		vkGetSwapchainImagesKHR(device, m_SwapChain, &imageCount, NULL);
		m_Images.resize(imageCount);
		imageCopies.resize(imageCount);
		vkGetSwapchainImagesKHR(device, m_SwapChain, &imageCount, imageCopies.data());

		// Copy over the images to the swapchain images
		for (size_t i = 0; i < imageCount; i++)
		{
			m_Images[i].Image = imageCopies[i];
			VulkanAllocator::TransitionImageLayout(m_Images[i].Image, m_ColourFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, 1);
		}

		for (uint32_t i = 0; i < imageCount; i++)
		{
			VkImageViewCreateInfo colorAttachmentView = {};
			colorAttachmentView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			colorAttachmentView.pNext = NULL;
			colorAttachmentView.format = m_ColourFormat;
			colorAttachmentView.image = m_Images[i].Image;
			colorAttachmentView.components = 
			{
				VK_COMPONENT_SWIZZLE_R,
				VK_COMPONENT_SWIZZLE_G,
				VK_COMPONENT_SWIZZLE_B,
				VK_COMPONENT_SWIZZLE_A
			};
			colorAttachmentView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			colorAttachmentView.subresourceRange.baseMipLevel = 0;
			colorAttachmentView.subresourceRange.levelCount = 1;
			colorAttachmentView.subresourceRange.baseArrayLayer = 0;
			colorAttachmentView.subresourceRange.layerCount = 1;
			colorAttachmentView.viewType = VK_IMAGE_VIEW_TYPE_2D;
			colorAttachmentView.flags = 0;

			vkCreateImageView(device, &colorAttachmentView, nullptr, &m_Images[i].ImageView);
		}

		VkFormat depthFormat = VulkanAllocator::FindDepthFormat();
		m_DepthStencil.MemoryAlloc = VulkanAllocator::CreateImage(width, height, 1, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VMA_MEMORY_USAGE_GPU_ONLY, m_DepthStencil.Image);

		m_DepthStencil.ImageView = VulkanAllocator::CreateImageView(m_DepthStencil.Image, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);
		VulkanAllocator::TransitionImageLayout(m_DepthStencil.Image, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1);

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Synchronization Objects
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		if (m_ImageAvailableSemaphores.empty())
		{
			m_ImageAvailableSemaphores.resize(framesInFlight);

			VkSemaphoreCreateInfo semaphoreInfo = {};
			semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

			VkFenceCreateInfo fenceInfo = {};
			fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

			for (size_t i = 0; i < framesInFlight; i++)
			{
				if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &m_ImageAvailableSemaphores[i]) != VK_SUCCESS)
				{
					LV_LOG_ERROR("Failed to create synchronization objects for a frame!");
				}
			}
		}
	}

	void VulkanSwapChain::Destroy()
	{
		auto device = m_Device->GetVulkanDevice();
		vkDeviceWaitIdle(device);

		if (m_SwapChain)
			vkDestroySwapchainKHR(device, m_SwapChain, nullptr);

		for (auto& image : m_Images)
			vkDestroyImageView(device, image.ImageView, nullptr);

		VulkanAllocator::DestroyImage(m_DepthStencil.Image, m_DepthStencil.MemoryAlloc);
		vkDestroyImageView(device, m_DepthStencil.ImageView, nullptr);

		vkDestroyCommandPool(device, m_CommandPool, nullptr);

		if (m_RenderPass)
			vkDestroyRenderPass(device, m_RenderPass, nullptr);

		for (size_t i = 0; i < Renderer::GetSpecification().FramesInFlight; i++)
		{
			vkDestroySemaphore(device, m_ImageAvailableSemaphores[i], nullptr);
		}

		vkDeviceWaitIdle(device);
	}

	void VulkanSwapChain::BeginFrame()
	{
		m_AquiredImage = AcquireNextImage();
	}

	void VulkanSwapChain::EndFrame()
	{
		VkSemaphore lastCmdBufSemaphore = VulkanRenderCommandBuffer::GetSemaphore();

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &lastCmdBufSemaphore;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &m_SwapChain;
		presentInfo.pImageIndices = &m_AquiredImage;
		presentInfo.pResults = nullptr; // Optional

		VkResult result = VK_SUCCESS;
		{
			LV_PROFILE_SCOPE("QueuePresent");
			result = vkQueuePresentKHR(m_Device->GetPresentQueue(), &presentInfo);
		}

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
		{
			auto& window = Application::Get().GetWindow();
			OnResize(window.GetWidth(), window.GetHeight(), window.IsVSync());
		}
		else if (result != VK_SUCCESS)
		{
			LV_LOG_ERROR("Failed to present swap chain image!");
		}

		m_CurrentFrame = (m_CurrentFrame + 1) % Renderer::GetSpecification().FramesInFlight;

		vkDeviceWaitIdle(m_Device->GetVulkanDevice());
	}

	void VulkanSwapChain::OnResize(uint32_t width, uint32_t height, const bool vsync)
	{
		auto device = m_Device->GetVulkanDevice();

		vkDeviceWaitIdle(device);
		Init(width, height, vsync);
		vkDeviceWaitIdle(device);
	}

	std::vector<VkImageView> VulkanSwapChain::GetImageViews()
	{
		std::vector<VkImageView> views = { };
		views.clear();

		for (auto& image : m_Images)
			views.push_back(image.ImageView);

		return views;
	}

	uint32_t VulkanSwapChain::AcquireNextImage()
	{
		LV_PROFILE_SCOPE("AquireNextImage");
		uint32_t imageIndex = 0;

		VkResult result = vkAcquireNextImageKHR(m_Device->GetVulkanDevice(), m_SwapChain, UINT64_MAX, m_ImageAvailableSemaphores[m_CurrentFrame], VK_NULL_HANDLE, &imageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			auto& window = Application::Get().GetWindow();
			OnResize(window.GetWidth(), window.GetHeight(), window.IsVSync());
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			LV_LOG_ERROR("Failed to acquire swap chain image!");
		}

		return imageIndex;
	}

	void VulkanSwapChain::FindImageFormatAndColorSpace()
	{
		VkPhysicalDevice physicalDevice = m_Device->GetPhysicalDevice()->GetVulkanPhysicalDevice();
		VkSurfaceKHR& surface = RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetVulkanSurface();

		// Get list of supported surface formats
		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, NULL);

		std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, surfaceFormats.data());

		// If the surface format list only includes one entry with VK_FORMAT_UNDEFINED,
		// there is no preferered format, so we assume VK_FORMAT_B8G8R8A8_UNORM
		if ((formatCount == 1) && (surfaceFormats[0].format == VK_FORMAT_UNDEFINED))
		{
			m_ColourFormat = VK_FORMAT_B8G8R8A8_UNORM;
			m_ColourSpace = surfaceFormats[0].colorSpace;
		}
		else
		{
			bool found_B8G8R8A8_UNORM = false;
			for (auto&& surfaceFormat : surfaceFormats)
			{
				if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM)
				{
					m_ColourFormat = surfaceFormat.format;
					m_ColourSpace = surfaceFormat.colorSpace;
					found_B8G8R8A8_UNORM = true;
					break;
				}
			}

			// in case VK_FORMAT_B8G8R8A8_UNORM is not available
			// select the first available color format
			if (!found_B8G8R8A8_UNORM)
			{
				m_ColourFormat = surfaceFormats[0].format;
				m_ColourSpace = surfaceFormats[0].colorSpace;
			}
		}
	}

	Ref<VulkanSwapChain> VulkanSwapChain::Create(VkInstance vkInstance, Ref<VulkanDevice> vkDevice)
	{
		return RefHelper::Create<VulkanSwapChain>(vkInstance, vkDevice);
	}
}