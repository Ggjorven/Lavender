#include "lvpch.h"
#include "VulkanRenderPass.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/APIs/Vulkan/VulkanManager.hpp"
#include "Lavender/APIs/Vulkan/Setup/VulkanHelper.hpp"

namespace Lavender
{

	VulkanRenderPass::VulkanRenderPass(const glm::vec2& extent, ColourSpace colourSpace, Attachments attachments)
		: m_ColourSpace(colourSpace), m_Attachments(attachments)
	{
		CreateRenderPass(extent, colourSpace, attachments);
		CreateFrameBuffers(extent, colourSpace, attachments);
	}

	VulkanRenderPass::VulkanRenderPass(const glm::vec2& extent, ColourSpace colourSpace, int attachments)
		: VulkanRenderPass(extent, colourSpace, (Attachments)attachments)
	{
	}

	VulkanRenderPass::VulkanRenderPass(const glm::vec2& extent, ColourSpace colourSpace, Attachments attachments, std::vector<VkFramebuffer>& framebuffers)
		: m_ColourSpace(colourSpace), m_Attachments(attachments)
	{
		CreateRenderPass(extent, colourSpace, attachments);
		m_SwapChainFramebuffers = framebuffers;
	}

	VulkanRenderPass::VulkanRenderPass(const glm::vec2& extent, ColourSpace colourSpace, int attachments, std::vector<VkFramebuffer>& framebuffers)
		: VulkanRenderPass(extent, colourSpace, (Attachments)attachments, framebuffers)
	{
	}

	VulkanRenderPass::~VulkanRenderPass()
	{
		VulkanDeviceInfo& info = VulkanManager::GetDeviceInfo();

		vkDeviceWaitIdle(info.Device);

		for (size_t i = 0; i < m_SwapChainFramebuffers.size(); i++)
			vkDestroyFramebuffer(info.Device, m_SwapChainFramebuffers[i], nullptr);

		vkDestroyRenderPass(info.Device, m_RenderPass, nullptr);
	}

	void VulkanRenderPass::RecreateFrameBuffers()
	{
		VulkanDeviceInfo& info = VulkanManager::GetDeviceInfo();

		for (size_t i = 0; i < m_SwapChainFramebuffers.size(); i++)
			vkDestroyFramebuffer(info.Device, m_SwapChainFramebuffers[i], nullptr);

		CreateFrameBuffers({ VulkanManager::GetSwapChainInfo().SwapChainExtent.width, VulkanManager::GetSwapChainInfo().SwapChainExtent.height }, m_ColourSpace, m_Attachments);
	}

	void VulkanRenderPass::CreateRenderPass(const glm::vec2& extent, ColourSpace colourSpace, Attachments attachments)
	{
		VkFormat format = VK_FORMAT_UNDEFINED;
		if (colourSpace == ColourSpace::Unorm) format = VK_FORMAT_B8G8R8A8_UNORM;
		else if (colourSpace == ColourSpace::sRGB) format = VK_FORMAT_B8G8R8A8_SRGB;
		
		// Colour
		VkAttachmentDescription colorAttachment = {};
		colorAttachment.format = format;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		// Depth
		VkAttachmentDescription depthAttachment = {};
		depthAttachment.format = VulkanHelper::FindDepthFormat();
		depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentRef = {};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		// Subpass
		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		if (attachments & DepthAttachment) subpass.pDepthStencilAttachment = &depthAttachmentRef;

		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		if (attachments & DepthAttachment) dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		else dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		std::vector<VkAttachmentDescription> attachmentsDes = {};

		if (attachments & DepthAttachment)
			attachmentsDes = { colorAttachment, depthAttachment };
		else
			attachmentsDes = { colorAttachment };

		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachmentsDes.size());
		renderPassInfo.pAttachments = attachmentsDes.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		if (vkCreateRenderPass(VulkanManager::GetDeviceInfo().Device, &renderPassInfo, nullptr, &m_RenderPass) != VK_SUCCESS)
			LV_LOG_ERROR("Failed to create render pass!");
	}

	void VulkanRenderPass::CreateFrameBuffers(const glm::vec2& extent, ColourSpace colourSpace, Attachments attachments)
	{
		std::vector<VkImageView> swapChainImageViews = VulkanManager::GetSwapChainInfo().SwapChainImageViews;
		VkImageView depthImageView = VulkanManager::GetResourceInfo().DepthImageView;

		m_SwapChainFramebuffers.resize(swapChainImageViews.size());

		for (size_t i = 0; i < swapChainImageViews.size(); i++)
		{
			std::vector<VkImageView> attachmentViews = { };

			if (attachments & DepthAttachment)
				attachmentViews = { swapChainImageViews[i], depthImageView };
			else
				attachmentViews = { swapChainImageViews[i] };

			VkFramebufferCreateInfo framebufferInfo = {};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = m_RenderPass;
			framebufferInfo.attachmentCount = static_cast<uint32_t>(attachmentViews.size());
			framebufferInfo.pAttachments = attachmentViews.data();
			framebufferInfo.width = (uint32_t)extent.x;
			framebufferInfo.height = (uint32_t)extent.y;
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(VulkanManager::GetDeviceInfo().Device, &framebufferInfo, nullptr, &m_SwapChainFramebuffers[i]) != VK_SUCCESS)
				LV_LOG_ERROR("Failed to create framebuffer!");
		}
	}

}