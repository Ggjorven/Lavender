#include "lvpch.h"
#include "VulkanSwapChain.hpp"

#include "Lavender/Core/Logging.hpp"
#include "Lavender/Core/Application.hpp"

#include "Lavender/Utils/Utils.hpp"
#include "Lavender/Utils/Profiler.hpp"

#include "Lavender/Renderer/Renderer.hpp"
#include "Lavender/APIs/Vulkan/VulkanContext.hpp"

namespace Lavender
{

	VulkanSwapChain::VulkanSwapChain(VkInstance vkInstance, std::shared_ptr<VulkanDevice> vkDevice)
		: m_Instance(vkInstance), m_Device(vkDevice)
	{
		FindImageFormatAndColorSpace();
	}

	void VulkanSwapChain::Init(uint32_t width, uint32_t height, const bool vsync)
	{
		VkDevice device = m_Device->GetVulkanDevice();
		VkPhysicalDevice physicalDevice = m_Device->GetPhysicalDevice()->GetVulkanPhysicalDevice();
		VkSurfaceKHR& surface = Utils::As<VulkanContext>(Renderer::GetContext())->GetVulkanSurface();

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
		swapchainCI.imageFormat = m_ColorFormat;
		swapchainCI.imageColorSpace = m_ColorSpace;
		swapchainCI.imageExtent = { swapchainExtent.width, swapchainExtent.height };
		swapchainCI.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
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
			vkDestroySwapchainKHR(device, oldSwapchain, nullptr);

		for (auto& image : m_Images)
			vkDestroyImageView(device, image.ImageView, nullptr);
		m_Images.clear();

		// Get the swap chain images
		static uint32_t imageCount = 0;
		static std::vector<VkImage> imageCopies = { };

		vkGetSwapchainImagesKHR(device, m_SwapChain, &imageCount, NULL);
		m_Images.resize(imageCount);
		imageCopies.resize(imageCount);
		vkGetSwapchainImagesKHR(device, m_SwapChain, &imageCount, imageCopies.data());

		// Copy over the images to the swapchain images
		for (size_t i = 0; i < imageCount; i++)
			m_Images[i].Image = imageCopies[i];

		for (uint32_t i = 0; i < imageCount; i++)
		{
			VkImageViewCreateInfo colorAttachmentView = {};
			colorAttachmentView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			colorAttachmentView.pNext = NULL;
			colorAttachmentView.format = m_ColorFormat;
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

		//
		// TODO: EVerything below
		//
		/*

		// Create command buffers
		{
			for (auto& commandBuffer : m_CommandBuffers)
				vkDestroyCommandPool(device, commandBuffer.CommandPool, nullptr);

			VkCommandPoolCreateInfo cmdPoolInfo = {};
			cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			cmdPoolInfo.queueFamilyIndex = m_QueueNodeIndex;
			cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;

			VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
			commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			commandBufferAllocateInfo.commandBufferCount = 1;

			m_CommandBuffers.resize(m_ImageCount);
			for (auto& commandBuffer : m_CommandBuffers)
			{
				VK_CHECK_RESULT(vkCreateCommandPool(device, &cmdPoolInfo, nullptr, &commandBuffer.CommandPool));

				commandBufferAllocateInfo.commandPool = commandBuffer.CommandPool;
				VK_CHECK_RESULT(vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, &commandBuffer.CommandBuffer));
			}
		}

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Synchronization Objects
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		if (!m_Semaphores.RenderComplete || !m_Semaphores.PresentComplete)
		{
			VkSemaphoreCreateInfo semaphoreCreateInfo{};
			semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			VK_CHECK_RESULT(vkCreateSemaphore(m_Device->GetVulkanDevice(), &semaphoreCreateInfo, nullptr, &m_Semaphores.RenderComplete));
			VKUtils::SetDebugUtilsObjectName(device, VK_OBJECT_TYPE_SEMAPHORE, "Swapchain Semaphore RenderComplete", m_Semaphores.RenderComplete);
			VK_CHECK_RESULT(vkCreateSemaphore(m_Device->GetVulkanDevice(), &semaphoreCreateInfo, nullptr, &m_Semaphores.PresentComplete));
			VKUtils::SetDebugUtilsObjectName(device, VK_OBJECT_TYPE_SEMAPHORE, "Swapchain Semaphore PresentComplete", m_Semaphores.PresentComplete);
		}

		if (m_WaitFences.size() != m_ImageCount)
		{
			VkFenceCreateInfo fenceCreateInfo{};
			fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

			m_WaitFences.resize(m_ImageCount);
			for (auto& fence : m_WaitFences)
			{
				VK_CHECK_RESULT(vkCreateFence(m_Device->GetVulkanDevice(), &fenceCreateInfo, nullptr, &fence));
				VKUtils::SetDebugUtilsObjectName(device, VK_OBJECT_TYPE_FENCE, "Swapchain Fence", fence);
			}
		}

		VkPipelineStageFlags pipelineStageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

		m_SubmitInfo = {};
		m_SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		m_SubmitInfo.pWaitDstStageMask = &pipelineStageFlags;
		m_SubmitInfo.waitSemaphoreCount = 1;
		m_SubmitInfo.pWaitSemaphores = &m_Semaphores.PresentComplete;
		m_SubmitInfo.signalSemaphoreCount = 1;
		m_SubmitInfo.pSignalSemaphores = &m_Semaphores.RenderComplete;


		VkFormat depthFormat = m_Device->GetPhysicalDevice()->GetDepthFormat();

		// Render Pass
		VkAttachmentDescription colorAttachmentDesc = {};
		// Color attachment
		colorAttachmentDesc.format = m_ColorFormat;
		colorAttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorReference = {};
		colorReference.attachment = 0;
		colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthReference = {};
		depthReference.attachment = 1;
		depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpassDescription = {};
		subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassDescription.colorAttachmentCount = 1;
		subpassDescription.pColorAttachments = &colorReference;
		subpassDescription.inputAttachmentCount = 0;
		subpassDescription.pInputAttachments = nullptr;
		subpassDescription.preserveAttachmentCount = 0;
		subpassDescription.pPreserveAttachments = nullptr;
		subpassDescription.pResolveAttachments = nullptr;

		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = 1;
		renderPassInfo.pAttachments = &colorAttachmentDesc;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpassDescription;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		VK_CHECK_RESULT(vkCreateRenderPass(m_Device->GetVulkanDevice(), &renderPassInfo, nullptr, &m_RenderPass));
		VKUtils::SetDebugUtilsObjectName(device, VK_OBJECT_TYPE_RENDER_PASS, "Swapchain render pass", m_RenderPass);

		// Create framebuffers for every swapchain image
		{
			for (auto& framebuffer : m_Framebuffers)
				vkDestroyFramebuffer(device, framebuffer, nullptr);

			VkFramebufferCreateInfo frameBufferCreateInfo = {};
			frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			frameBufferCreateInfo.renderPass = m_RenderPass;
			frameBufferCreateInfo.attachmentCount = 1;
			frameBufferCreateInfo.width = m_Width;
			frameBufferCreateInfo.height = m_Height;
			frameBufferCreateInfo.layers = 1;

			m_Framebuffers.resize(m_ImageCount);
			for (uint32_t i = 0; i < m_Framebuffers.size(); i++)
			{
				frameBufferCreateInfo.pAttachments = &m_Images[i].ImageView;
				VK_CHECK_RESULT(vkCreateFramebuffer(m_Device->GetVulkanDevice(), &frameBufferCreateInfo, nullptr, &m_Framebuffers[i]));
				VKUtils::SetDebugUtilsObjectName(m_Device->GetVulkanDevice(), VK_OBJECT_TYPE_FRAMEBUFFER, fmt::format("Swapchain framebuffer (Frame in flight: {})", i), m_Framebuffers[i]);
			}
		}
		*/
	}

	void VulkanSwapChain::Destroy()
	{
		auto device = m_Device->GetVulkanDevice();
		vkDeviceWaitIdle(device);

		if (m_SwapChain)
			vkDestroySwapchainKHR(device, m_SwapChain, nullptr);

		for (auto& image : m_Images)
			vkDestroyImageView(device, image.ImageView, nullptr);

		//for (auto& commandBuffer : m_CommandBuffers)
		//	vkDestroyCommandPool(device, commandBuffer.CommandPool, nullptr);

		if (m_RenderPass)
			vkDestroyRenderPass(device, m_RenderPass, nullptr);

		for (auto framebuffer : m_Framebuffers)
			vkDestroyFramebuffer(device, framebuffer, nullptr);

		//for (size_t i = 0; i < Renderer::GetSpecification().FramesInFlight; i++)
		//{
		//	vkDestroySemaphore(device, m_RenderFinishedSemaphores[i], nullptr);
		//	vkDestroySemaphore(device, m_ImageAvailableSemaphores[i], nullptr);
		//	vkDestroyFence(device, m_InFlightFences[i], nullptr);
		//}

		vkDeviceWaitIdle(device);
	}

	void VulkanSwapChain::OnResize(uint32_t width, uint32_t height, const bool vsync)
	{
		auto device = m_Device->GetVulkanDevice();

		vkDeviceWaitIdle(device);
		Init(width, height, vsync);
		vkDeviceWaitIdle(device);
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
		VkSurfaceKHR& surface = Utils::As<VulkanContext>(Renderer::GetContext())->GetVulkanSurface();

		// Get list of supported surface formats
		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, NULL);

		std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, surfaceFormats.data());

		// If the surface format list only includes one entry with VK_FORMAT_UNDEFINED,
		// there is no preferered format, so we assume VK_FORMAT_B8G8R8A8_UNORM
		if ((formatCount == 1) && (surfaceFormats[0].format == VK_FORMAT_UNDEFINED))
		{
			m_ColorFormat = VK_FORMAT_B8G8R8A8_UNORM;
			m_ColorSpace = surfaceFormats[0].colorSpace;
		}
		else
		{
			bool found_B8G8R8A8_UNORM = false;
			for (auto&& surfaceFormat : surfaceFormats)
			{
				if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM)
				{
					m_ColorFormat = surfaceFormat.format;
					m_ColorSpace = surfaceFormat.colorSpace;
					found_B8G8R8A8_UNORM = true;
					break;
				}
			}

			// in case VK_FORMAT_B8G8R8A8_UNORM is not available
			// select the first available color format
			if (!found_B8G8R8A8_UNORM)
			{
				m_ColorFormat = surfaceFormats[0].format;
				m_ColorSpace = surfaceFormats[0].colorSpace;
			}
		}
	}

	std::shared_ptr<VulkanSwapChain> VulkanSwapChain::Create(VkInstance vkInstance, std::shared_ptr<VulkanDevice> vkDevice)
	{
		return std::make_shared<VulkanSwapChain>(vkInstance, vkDevice);
	}
}