#include "lvpch.h"
#include "VulkanViewport.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/Renderer/Renderer.hpp"

#include "Lavender/APIs/Vulkan/VulkanAllocator.hpp"
#include "Lavender/APIs/Vulkan/VulkanContext.hpp"
#include "Lavender/APIs/Vulkan/VulkanRenderPass.hpp"

#include <imgui_internal.h>
#include <backends/imgui_impl_vulkan.h>

namespace Lavender
{

	VulkanViewportImage::VulkanViewportImage(uint32_t width, uint32_t height)
		: m_Miplevels(1)
	{
		auto context = RefHelper::RefAs<VulkanContext>(Renderer::GetContext());

		auto imageViews = context->GetSwapChain()->GetImageViews();
		m_Images.resize(imageViews.size());
		for (size_t i = 0; i < imageViews.size(); i++)
		{
			m_Images[i].Allocation = VulkanAllocator::CreateImage(width, height, m_Miplevels, context->GetSwapChain()->GetColourFormat(), VK_IMAGE_TILING_LINEAR, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VMA_MEMORY_USAGE_GPU_ONLY, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_Images[i].Image);

			m_Images[i].ImageView = VulkanAllocator::CreateImageView(m_Images[i].Image, context->GetSwapChain()->GetColourFormat(), VK_IMAGE_ASPECT_COLOR_BIT, m_Miplevels);

			VulkanAllocator::TransitionImageLayout(m_Images[i].Image, context->GetSwapChain()->GetColourFormat(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, m_Miplevels);
		}

		m_Sampler = VulkanAllocator::CreateSampler(m_Miplevels); // TODO: Change to 0?
	}

	VulkanViewportImage::~VulkanViewportImage()
	{
		auto device = RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetLogicalDevice()->GetVulkanDevice();
		vkDeviceWaitIdle(device);

		for (auto& image : m_Images)
		{
			VulkanAllocator::DestroyImage(image.Image, image.Allocation);
			vkDestroyImageView(device, image.ImageView, nullptr);
		}

		vkDestroySampler(device, m_Sampler, nullptr);
	}

	void VulkanViewportImage::Resize(uint32_t width, uint32_t height)
	{
		m_Miplevels = static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1;
		
		auto context = RefHelper::RefAs<VulkanContext>(Renderer::GetContext());
		auto device = context->GetLogicalDevice()->GetVulkanDevice();

		for (auto& image : m_Images)
		{
			VulkanAllocator::DestroyImage(image.Image, image.Allocation);
			vkDestroyImageView(device, image.ImageView, nullptr);
		}

		auto imageViews = context->GetSwapChain()->GetImageViews();
		m_Images.resize(imageViews.size());
		for (size_t i = 0; i < imageViews.size(); i++)
		{
			m_Images[i].Allocation = VulkanAllocator::CreateImage(width, height, m_Miplevels, context->GetSwapChain()->GetColourFormat(), VK_IMAGE_TILING_LINEAR, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VMA_MEMORY_USAGE_GPU_ONLY, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_Images[i].Image);

			m_Images[i].ImageView = VulkanAllocator::CreateImageView(m_Images[i].Image, context->GetSwapChain()->GetColourFormat(), VK_IMAGE_ASPECT_COLOR_BIT, m_Miplevels);
		}
	}



	VulkanViewportRenderPass::VulkanViewportRenderPass(Ref<ViewportImage> image)
		: m_Image(RefHelper::RefAs<VulkanViewportImage>(image)), m_CommandBuffer(RefHelper::RefAs<VulkanRenderCommandBuffer>(RenderCommandBuffer::Create(RenderCommandBuffer::Usage::Sequential)))
	{
		auto context = RefHelper::RefAs<VulkanContext>(Renderer::GetContext());

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Renderpass
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		std::vector<VkAttachmentDescription> attachments = { };
		std::vector<VkAttachmentReference> attachmentRefs = { };

		{
			VkAttachmentDescription& colorAttachment = attachments.emplace_back();
			colorAttachment.format = context->GetSwapChain()->GetColourFormat();
			colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
			colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
			colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			colorAttachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

			VkAttachmentReference& colorAttachmentRef = attachmentRefs.emplace_back();
			colorAttachmentRef.attachment = 0;
			colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		}

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &attachmentRefs[0];

		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0; // or VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = (uint32_t)attachments.size();
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		if (vkCreateRenderPass(context->GetLogicalDevice()->GetVulkanDevice(), &renderPassInfo, nullptr, &m_RenderPass) != VK_SUCCESS)
			LV_LOG_ERROR("Failed to create render pass!");

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Framebuffers // TODO: Framebuffer class
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		auto images = m_Image->GetImages();
		m_Framebuffers.resize(images.size());
		for (size_t i = 0; i < images.size(); i++)
		{
			std::vector<VkImageView> attachments = { };
			attachments.push_back(images[i].ImageView);

			VkFramebufferCreateInfo framebufferInfo = {};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = m_RenderPass;
			framebufferInfo.attachmentCount = (uint32_t)attachments.size();
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = Application::Get().GetWindow().GetWidth();
			framebufferInfo.height = Application::Get().GetWindow().GetHeight();
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(context->GetLogicalDevice()->GetVulkanDevice(), &framebufferInfo, nullptr, &m_Framebuffers[i]) != VK_SUCCESS)
				LV_LOG_ERROR("Failed to create framebuffer!");
		}
	}

	VulkanViewportRenderPass::~VulkanViewportRenderPass()
	{
		auto device = RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetLogicalDevice();
		vkDeviceWaitIdle(device->GetVulkanDevice());

		for (auto& framebuffer : m_Framebuffers)
			vkDestroyFramebuffer(device->GetVulkanDevice(), framebuffer, nullptr);

		vkDestroyRenderPass(device->GetVulkanDevice(), m_RenderPass, nullptr);
	}

	void VulkanViewportRenderPass::Begin()
	{
		m_CommandBuffer->Begin();

		VkExtent2D swapChainExtent = { Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight() };

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_RenderPass;
		renderPassInfo.framebuffer = m_Framebuffers[RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetSwapChain()->GetAquiredImage()];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = swapChainExtent;

		std::vector<VkClearValue> clearValues = {};

		VkClearValue colourClear = { { {0.0f, 0.0f, 0.0f, 1.0f} } };
		clearValues.push_back(colourClear);

		renderPassInfo.clearValueCount = (uint32_t)clearValues.size();
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(m_CommandBuffer->GetVulkanCommandBuffer(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)swapChainExtent.width;
		viewport.height = (float)swapChainExtent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(m_CommandBuffer->GetVulkanCommandBuffer(), 0, 1, &viewport);

		VkRect2D scissor = {};
		scissor.offset = { 0, 0 };
		scissor.extent = swapChainExtent;
		vkCmdSetScissor(m_CommandBuffer->GetVulkanCommandBuffer(), 0, 1, &scissor);
	}

	void VulkanViewportRenderPass::End()
	{
		vkCmdEndRenderPass(m_CommandBuffer->GetVulkanCommandBuffer());

		m_CommandBuffer->End();
	}

	void VulkanViewportRenderPass::Submit()
	{
		m_CommandBuffer->Submit();
	}

	void VulkanViewportRenderPass::Resize(uint32_t width, uint32_t height)
	{
		m_Image->Resize(width, height);

		auto context = RefHelper::RefAs<VulkanContext>(Renderer::GetContext());
		auto device = context->GetLogicalDevice();

		// Destroy
		for (auto& framebuffer : m_Framebuffers)
			vkDestroyFramebuffer(device->GetVulkanDevice(), framebuffer, nullptr);

		auto imageViews = context->GetSwapChain()->GetImageViews();
		m_Framebuffers.resize(imageViews.size());
		for (size_t i = 0; i < imageViews.size(); i++)
		{
			std::vector<VkImageView> attachments = { };
			attachments.push_back(context->GetSwapChain()->GetImageViews()[i]);

			if (m_Specification.UsedAttachments & RenderPassSpecification::Attachments::Depth)
			{
				auto depthImageView = context->GetSwapChain()->GetDepthImageView();
				attachments.push_back(depthImageView);
			}

			VkFramebufferCreateInfo framebufferInfo = {};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = m_RenderPass;
			framebufferInfo.attachmentCount = (uint32_t)attachments.size();
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = Application::Get().GetWindow().GetWidth();
			framebufferInfo.height = Application::Get().GetWindow().GetHeight();
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(context->GetLogicalDevice()->GetVulkanDevice(), &framebufferInfo, nullptr, &m_Framebuffers[i]) != VK_SUCCESS)
				LV_LOG_ERROR("Failed to create framebuffer!");
		}
	}

	Ref<RenderPass> VulkanViewportRenderPass::GetRenderPass()
	{
		return RefHelper::Create<VulkanRenderPass>(m_RenderPass);
	}



	VulkanViewport::VulkanViewport(uint32_t width, uint32_t height)
	{
		auto image = RefHelper::Create<VulkanViewportImage>(width, height);
		m_Renderpass = RefHelper::Create<VulkanViewportRenderPass>(image);

		for (auto& i : image->GetImages())
		{
			auto img = ImGui_ImplVulkan_AddTexture(image->GetSampler(), i.ImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
			m_ImGuiImages.push_back((ImTextureID)img);
		}
	}

	VulkanViewport::~VulkanViewport()
	{
		// TODO: Destroy ImGui Textures
	}

	void VulkanViewport::BeginFrame()
	{
		m_Renderpass->Begin();
	}

	void VulkanViewport::EndFrame()
	{
		m_Renderpass->End();
		m_Renderpass->Submit();
	}

	void VulkanViewport::Resize(uint32_t width, uint32_t height)
	{
		// TODO: Destroy ImGui Textures
		//auto image = m_Renderpass->GetImage();
		//for (auto& i : image->GetImages())
		//{
		//	auto img = ImGui_ImplVulkan_AddTexture(image->GetSampler(), i.ImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		//	m_ImGuiImages.push_back((ImTextureID)img);
		//}

		m_Renderpass->Resize(width, height);
	}

	ImTextureID VulkanViewport::GetCurrentImGuiTexture()
	{
		auto acquiredImage = RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetSwapChain()->GetAquiredImage();
		return m_ImGuiImages[acquiredImage];
	}

}